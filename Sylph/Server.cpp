#include "Server.hpp"
#include "Debug.hpp"
#include <cstring>

using namespace KapMirror::Sylph;

Server::~Server() { close(); }

void Server::close() {
    if (listener != nullptr) {
        listener->close();
    }
    listener = nullptr;
}

void Server::start(int port) {
    if (port < 0 || port > 65535) {
        throw std::runtime_error("Invalid port number");
    }

    auto address = std::make_shared<Address>(port, false, KapMirror::Address::SocketType::UDP);
    listener = std::make_shared<UdpListener>(address);

    try {
        listener->start();
    } catch (SocketException& e) {
        KapEngine::Debug::error("Server: Failed to start listener: " + std::string(e.what()));
        return;
    }
}

void Server::disconnect(int connectionId) {
    std::shared_ptr<ServerConnection> connection;
    if (connections.tryGetValue(connectionId, connection)) {
        connection->disconnect();
    }
}

void Server::send(int connectionId, const std::shared_ptr<ArraySegment<byte>>& message) {
    std::shared_ptr<ServerConnection> connection;
    if (connections.tryGetValue(connectionId, connection)) {
        connection->send(message);
    }
}

void Server::tick() { tickIncoming(); }

void Server::tickIncoming() {
    while (listener->isReadable()) {
        try {
            // Clean buffer
            std::memset(rawReceiveBuffer, 0, MTU_DEF);

            auto clientAddress = Address::createAddress();
            int msgLength = 0;
            int connectionId;
            if (listener->receiveFrom(clientAddress, MTU_DEF, rawReceiveBuffer, msgLength)) {
                connectionId = getConnectionId(clientAddress);

                if (msgLength <= MTU_DEF) {
                    std::shared_ptr<ServerConnection> connection;
                    if (!connections.tryGetValue(connectionId, connection)) {
                        KapEngine::Debug::log("Server: New connection from " + clientAddress->toString());

                        connection = std::make_shared<ServerConnection>(connectionId, clientAddress, listener);

                        connection->onAuthenticated = [this](Connection& con) {
                            con.sendHandshake();

                            if (onConnected != nullptr) {
                                onConnected(*this, con.getConnectionId());
                            }
                        };

                        connection->onData = [this](Connection& con, const std::shared_ptr<ArraySegment<byte>>& message) {
                            if (onData != nullptr) {
                                onData(*this, con.getConnectionId(), message);
                            }
                        };

                        connection->onDisconnected = [this](Connection& con) {
                            connectionsToRemove.push_back(con.getConnectionId());

                            if (onDisconnected != nullptr) {
                                onDisconnected(*this, con.getConnectionId());
                            }
                        };

                        connections[connectionId] = connection;
                    }

                    connection->rawInput(rawReceiveBuffer, msgLength);
                } else {
                    KapEngine::Debug::error("Server: message of size " + std::to_string(msgLength) + " does not fit into buffer of size " +
                                            std::to_string(MTU_DEF) + ". The excess was silently dropped. Disconnecting connectionId=" +
                                            std::to_string(connectionId) + ".");
                    disconnect(connectionId);
                }
            }
        } catch (SocketException& e) {
            KapEngine::Debug::error("Server: Exception=" + std::string(e.what()));
            return;
        }
    }

    for (auto connectionId : connectionsToRemove) {
        connections.remove(connectionId);
    }
    connectionsToRemove.clear();

    for (auto const& [id, conn] : connections) {
        conn->tick();
    }
}

int Server::getConnectionId(const std::shared_ptr<Address>& address) { return (int)std::hash<std::string>()(address->toString()); }
