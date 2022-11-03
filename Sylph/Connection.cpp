#include "Connection.hpp"
#include "KapMirror/Runtime/NetworkTime.hpp"
#include <iostream>

using namespace KapMirror::Sylph;

Connection::Connection(int _connectionId, std::shared_ptr<Address> _address, int _timeout) : connectionId(_connectionId), address(_address), timeout(_timeout) {
    state = ConnectionState::Connected;
    lastReceiveTime = NetworkTime::localTime();
}

int Connection::getConnectionId() const {
    return connectionId;
}

void Connection::disconnect() {
    if (state == ConnectionState::Disconnected) {
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

    std::cout << "Connection: Disposed." << std::endl;
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
    if (state == ConnectionState::Disconnected) {
        return;
    }

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

void Connection::tick() {
    if (state == ConnectionState::Disconnected) {
        return;
    }

    long long time = NetworkTime::localTime();
    switch (state) {
        case ConnectionState::Connected: {
            handleTimeout(time);
            break;
        }
        case ConnectionState::Authenticated: {
            handlePing(time);
            handleTimeout(time);
            break;
        }
    }
}

void Connection::handleOnConnected(MessageType type, std::shared_ptr<ArraySegment<byte>> message) {
    switch (type) {
        case MessageType::Handshake: {
            std::cout << "Connection: received handshake" << std::endl;
            state = ConnectionState::Authenticated;
            lastReceiveTime = NetworkTime::localTime();

            if (onAuthenticated != nullptr) {
                onAuthenticated(*this);
            }
            break;
        }
        case MessageType::Ping:
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
        case MessageType::Ping: {
            lastReceiveTime = NetworkTime::localTime();
            break;
        }
        case MessageType::Data: {
            if (message->getSize() > 0) {
                lastReceiveTime = NetworkTime::localTime();
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

void Connection::handlePing(long long time) {
    if (time >= lastPingTime + PING_INTERVAL) {
        sendPing();
        lastPingTime = time;
    }
}


void Connection::handleTimeout(long long time) {
    if (time >= lastReceiveTime + timeout) {
        std::cout << "Connection: Connection timed out after not receiving any message for " << timeout << "ms. Disconnecting." << std::endl;
        disconnect();
    }
}

void Connection::sendDisconnect() {
    rawSend(MessageType::Disconnect, nullptr, 0);
}

void Connection::sendPing() {
    rawSend(MessageType::Ping, nullptr, 0);
}
