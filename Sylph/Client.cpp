#include "Client.hpp"
#include "Debug.hpp"
#include <cstring>

using namespace KapMirror::Sylph;

Client::Client() { connected = false; }

Client::~Client() { dispose(); }

void Client::dispose() {
    connected = false;
    if (client != nullptr) {
        client->close();
    }

    // clean values
    client = nullptr;
    connection = nullptr;
}

bool Client::isConnected() const { return connected; }

void Client::connect(const std::string& ip, int port) {
    if (connected) {
        KapEngine::Debug::error("Client: Sylph Client can not create connection because an existing connection is connected!");
        return;
    }

    if (port < 0 || port > 65535) {
        throw std::runtime_error("Invalid port number");
    }

    auto address = Address::createAddress(ip, port, KapMirror::Address::SocketType::UDP);
    client = std::make_shared<UdpClient>(address);
    connection = std::make_shared<ClientConnection>(address, client);

    connection->onAuthenticated = [this](Connection& con) {
        connected = true;

        if (onConnected != nullptr) {
            onConnected(*this);
        }
    };

    connection->onData = [this](Connection& con, const std::shared_ptr<ArraySegment<byte>>& message) {
        if (onData != nullptr) {
            onData(*this, message);
        }
    };

    connection->onDisconnected = [this](Connection& con) {
        if (onDisconnected != nullptr) {
            onDisconnected(*this);
        }

        // Clean up
        connected = false;
        client->close();
    };

    connection->connect();
}

void Client::disconnect() {
    if (connected && connection != nullptr) {
        connection->disconnect();
    }
}

void Client::send(const std::shared_ptr<ArraySegment<byte>>& message) {
    if (connected) {
        connection->send(message);
    } else {
        KapEngine::Debug::error("Client: can't send because client not connected!");
    }
}

void Client::tick() { tickIncoming(); }

void Client::tickIncoming() {
    if (connection == nullptr) {
        return;
    }

    while (client->isReadable()) {
        try {
            // Clean buffer
            std::memset(rawReceiveBuffer, 0, MTU_DEF);

            int msgLength = 0;
            if (client->receive(MTU_DEF, rawReceiveBuffer, msgLength)) {
                if (msgLength <= MTU_DEF) {
                    connection->rawInput(rawReceiveBuffer, msgLength);
                } else {
                    KapEngine::Debug::error("Client: message of size " + std::to_string(msgLength) + " does not fit into buffer of size " +
                                            std::to_string(MTU_DEF) + ". The excess was silently dropped. Disconnecting.");
                    disconnect();
                }
            }
        } catch (SocketException& e) { KapEngine::Debug::error("Client: Exception=" + std::string(e.what())); }
    }

    connection->tick();
}
