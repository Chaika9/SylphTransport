#include "Server.hpp"
#include <iostream>

using namespace KapMirror::Sylph;

Server::Server() {
}

Server::~Server() {
    close();
}

void Server::close() {
    if (listener != nullptr) {
        listener->close();
        listener = nullptr;
    }
}

void Server::start(int port) {
    if (port < 0 || port > 65535) {
        throw std::runtime_error("Invalid port number");
    }

    auto address = std::make_shared<Address>(port, false, KapMirror::Sylph::Address::SocketType::UDP);
    listener = std::make_shared<UdpListener>(address);

    try {
        listener->start();
    } catch (SocketException& e) {
        std::cerr << "Server: Exception=" << e.what() << std::endl;
        return;
    }
}

void Server::disconnect(int connectionId) {
    std::shared_ptr<ServerConnection> connection;
    if (connections.tryGetValue(connectionId, connection)) {
        connection->disconnect();
    }
}

void Server::send(int connectionId, std::shared_ptr<ArraySegment<byte>> message) {
    std::shared_ptr<ServerConnection> connection;
    if (connections.tryGetValue(connectionId, connection)) {
        connection->send(message);
    }
}

void Server::tick() {
    tickIncoming();
}

void Server::tickIncoming() {
    while (listener->isReadable()) {
        try {
            auto clientAddress = Address::createAddress();
            int msgLength = 0;
            int connectionId;
            if (listener->receiveFrom(clientAddress, MTU_DEF, rawReceiveBuffer, msgLength)) {
                connectionId = getConnectionId(clientAddress);

                if (msgLength <= MTU_DEF) {
                    std::shared_ptr<ServerConnection> connection;
                    if (!connections.tryGetValue(connectionId, connection)) {
                        std::cout << "Server: New connection" << std::endl;

                        connection = std::make_shared<ServerConnection>(connectionId, clientAddress, listener);

                        connection->onAuthenticated = [this](Connection& con) {
                            con.sendHandshake();

                            if (onConnected != nullptr) {
                                onConnected(*this, con.getConnectionId());
                            }
                        };

                        connection->onData = [this](Connection& con, std::shared_ptr<ArraySegment<byte>> message) {
                            if (onData != nullptr) {
                                onData(*this, con.getConnectionId(), message);
                            }
                        };

                        connection->onDisconnected = [this](Connection& con) {
                            connections.remove(con.getConnectionId());

                            if (onDisconnected != nullptr) {
                                onDisconnected(*this, con.getConnectionId());
                            }
                        };

                        connections[connectionId] = connection;
                        connection->rawInput(rawReceiveBuffer, msgLength);
                    } else {
                        connection->rawInput(rawReceiveBuffer, msgLength);
                    }
                } else {
                    std::cerr << "Server: message of size " << msgLength << " does not fit into buffer of size " << MTU_DEF << ". The excess was silently dropped. Disconnecting connectionId=" << connectionId << ".";
                    disconnect(connectionId);
                }
            }
        } catch (SocketException& e) {
            std::cerr << "Server: Exception=" << e.what() << std::endl;
            return;
        }
    }

    for (auto const& [id, conn] : connections) {
        conn->tick();
    }
}

int Server::getConnectionId(std::shared_ptr<Address> address) {
    return (int) std::hash<std::string>()(address->toString());
}
