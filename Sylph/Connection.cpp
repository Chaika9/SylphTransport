#include "Connection.hpp"
#include <iostream>

using namespace KapMirror::Sylph;

Connection::Connection(int _connectionId, std::shared_ptr<Address> _address) : connectionId(_connectionId), address(_address) {
    state = ConnectionState::Connected;
}

int Connection::getConnectionId() const {
    return connectionId;
}

void Connection::disconnect() {
    if (state != ConnectionState::Connected) {
        return;
    }

    try {
        sendDisconnect();
    } catch (...) {
        // this is ok, the connection was already closed
    }

    std::cout << "Connection: Disconnected." << std::endl;
    state = ConnectionState::Disconnected;

    if (onDisconnected != nullptr) {
        onDisconnected(*this);
    }
}

void Connection::send(std::shared_ptr<ArraySegment<byte>> message) {
    if (message->getSize() <= 0) {
        std::cerr << "Connection: tried sending empty message. This should never happen. Disconnecting." << std::endl;
        disconnect();
        return;
    }

    rawSend(MessageType::Data, message->toArray(), message->getSize());
}

void Connection::sendHandshake() {
    std::cout << "Connection: sending Handshake to other end!" << std::endl;
    rawSend(MessageType::Handshake, nullptr, 0);
}

void Connection::rawInput(byte* buffer, int msgLength) {
    if (msgLength <= 0) {
        return;
    }

    byte type = buffer[0];
    auto message = ArraySegment<byte>::createArraySegment(buffer, 1, msgLength);

    if (state == ConnectionState::Connected) {
        handleOnConnected((MessageType)type, message);
    } else if (state == ConnectionState::Authenticated) {
        handleOnAuthenticated((MessageType)type, message);
    }
}

void Connection::handleOnConnected(MessageType type, std::shared_ptr<ArraySegment<byte>> message) {
    switch (type) {
        case MessageType::Handshake: {
            std::cout << "Connection: received handshake" << std::endl;
            state = ConnectionState::Authenticated;

            if (onAuthenticated != nullptr) {
                onAuthenticated(*this);
            }
            break;
        }
        case MessageType::Data:
        case MessageType::Disconnect: {
            std::cout << "Connection: received invalid type " << (int)type << " while Connected. Disconnecting the connection." << std::endl;
            disconnect();
            break;
        }
    }
}

void Connection::handleOnAuthenticated(MessageType type, std::shared_ptr<ArraySegment<byte>> message) {
    switch (type) {
        case MessageType::Handshake: {
            std::cout << "Connection: received invalid type " << (int)type << " while Authenticated. Disconnecting the connection." << std::endl;
            disconnect();
            break;
        }
        case MessageType::Data: {
            if (message->getSize() > 0) {
                if (onData != nullptr) {
                    onData(*this, message);
                }
            } else {
                std::cout << "Connection: received empty Data message while Authenticated. Disconnecting the connection." << std::endl;
                disconnect();
            }
            break;
        }
        case MessageType::Disconnect: {
            std::cout << "Connection: received disconnect message" << std::endl;
            disconnect();
            break;
        }
    }
}

void Connection::sendDisconnect() {
    rawSend(MessageType::Disconnect, nullptr, 0);
}
