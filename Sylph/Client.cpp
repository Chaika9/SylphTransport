#include "Client.hpp"
#include <iostream>

using namespace KapMirror::Sylph;

Client::Client() {
    connected = false;
}

Client::~Client() {
    if (client != nullptr) {
        dispose();
    }
}

void Client::dispose() {
    connected = false;
    client->close();

    // clean values
    client = nullptr;
    connection = nullptr;
}

void Client::connect(std::string ip, int port) {
    if (connected) {
        std::cerr << "Client: Sylph Client can not create connection because an existing connection is connected" << std::endl;
        return;
    }

    if (port < 0 || port > 65535) {
        throw std::runtime_error("Invalid port number");
    }

    auto address = Address::createAddress(ip, port, KapMirror::Sylph::Address::SocketType::UDP);
    client = std::make_shared<UdpClient>(address);
    connection = std::make_shared<ClientConnection>(address, client);

    connection->onAuthenticated = [this](Connection& con) {
        connected = true;

        if (onConnected != nullptr) {
            onConnected(*this);
        }
    };

    connection->onData = [this](Connection& con, std::shared_ptr<ArraySegment<byte>> message) {
        if (onData != nullptr) {
            onData(*this, message);
        }
    };

    connection->onDisconnected = [this](Connection& con) {
        connected = false;
        connection = nullptr;

        if (onDisconnected != nullptr) {
            onDisconnected(*this);
        }
    };

    connection->connect();
}

void Client::disconnect() {
    if (connected) {
        connection->disconnect();
    }
}

void Client::send(std::shared_ptr<ArraySegment<byte>> message) {
    if (connected) {
        connection->send(message);
    } else {
        std::cerr << "Client: can't send because client not connected!" << std::endl;
    }
}

void Client::tick() {
    tickIncoming();
}

void Client::tickIncoming() {
    if (connection == nullptr) {
        return;
    }

    while (client->isReadable()) {
        try {
            int msgLength = 0;
            if (client->receive(MTU_DEF, rawReceiveBuffer, msgLength)) {
                if (msgLength <= MTU_DEF) {
                    connection->rawInput(rawReceiveBuffer, msgLength);
                } else {
                    std::cerr << "Client: message of size " << msgLength << " does not fit into buffer of size " << MTU_DEF << ". The excess was silently dropped. Disconnecting.";
                    disconnect();
                }
            }
        } catch (SocketException& e) {
            std::cerr << "Client: Exception=" << e.what() << std::endl;
        }
    }

    connection->tick();
}
