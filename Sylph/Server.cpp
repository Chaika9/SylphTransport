#include "Server.hpp"
#include "KapMirror/Runtime/NetworkReader.hpp"
#include "KapMirror/Runtime/NetworkWriter.hpp"
#include <iostream>
#include <cstring>

using namespace KapMirror::Sylph;

Server::Server(int _maxMessageSize) : maxMessageSize(_maxMessageSize) {
}

Server::~Server() {
    close();
}

void Server::close() {
    running = false;
    if (listenerThread.joinable()) {
        listenerThread.join();
    }
    listener->close();
    receivePipe.clear();
}

void Server::start(int port) {
    if (port < 0 || port > 65535) {
        throw std::runtime_error("Invalid port number");
    }

    running = true;
    counter = 0;

    listenerThread = std::thread([this, port]() { this->listen(port); });
}

void Server::listen(int port) {
    auto address = std::make_shared<Address>(port);
    listener = std::make_shared<UdpListener>(address);

    try {
        listener->start();
    } catch (SocketException& e) {
        std::cerr << "Server: Exception=" << e.what() << std::endl;
        return;
    }

    while (running) {
        if (!listener->isReadable()) {
            // Don't burn too much CPU while idling
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        try {
            auto client = listener->acceptUdpClient();

            // Create a new connection object
            auto connection = std::make_shared<ClientConnection>();
            connection->id = ++counter;
            connection->client = client;
            connection->thread = std::thread([this, connection]() { this->handleConnection(connection); });

            std::lock_guard<std::mutex> lock(connectionListMutex);
            connectionList.push_back(connection);
        } catch (SocketException& e) {
            std::cout << "Server: Exception=" << e.what() << std::endl;
        }
    }

    // Wait for all connection threads to finish
    std::lock_guard<std::mutex> lock(connectionListMutex);
    for (auto& connection : connectionList) {
        if (connection->thread.joinable()) {
            connection->thread.join();
        }
    }
    connectionList.clear();
}

int Server::tick(int processLimit) {
    if (processLimit < 0) {
        throw std::runtime_error("Invalid process limit");
    }

    if (receivePipe.isEmpty()) {
        return 0;
    }

    for (int i = 0; i < processLimit; i++) {
        int connectionId;
        MagnificentReceivePipe::EventType eventType;
        std::shared_ptr<ArraySegment<byte>> message;
        if (receivePipe.pop(connectionId, eventType, message)) {
            switch (eventType) {
                case MagnificentReceivePipe::EventType::Connected:
                    if (onConnected) {
                        onConnected(connectionId);
                    }
                    break;
                case MagnificentReceivePipe::EventType::Disconnected:
                    if (onDisconnected) {
                        onDisconnected(connectionId);
                    }
                    disconnectClient(connectionId);
                    break;
                case MagnificentReceivePipe::EventType::Data:
                    if (onData) {
                        onData(connectionId, message);
                    }
                    break;
            }
        } else {
            break;
        }
    }
    return receivePipe.getSize();
}

void Server::disconnectClient(int clientId) {
    std::lock_guard<std::mutex> lock(connectionListMutex);
    if (connectionList.empty()) {
        return;
    }
    for (auto it = connectionList.begin(); it != connectionList.end(); it++) {
        std::shared_ptr<ClientConnection> connection = *it;
        if (connection->id == clientId) {
            connection->client->close();
            if (connection->thread.joinable()) {
                connection->thread.join();
            }
            connectionList.erase(it);
            break;
        }
    }
}

void Server::send(int clientId, std::shared_ptr<ArraySegment<byte>> message) {
    std::lock_guard<std::mutex> lock(connectionListMutex);
    if (connectionList.empty()) {
        return;
    }
    if (message->getSize() > maxMessageSize) {
        std::cerr << "Client: Message too large Size=" << message->getSize() << std::endl;
        return;
    }

    for (auto& connection : connectionList) {
        if (connection->id == clientId) {
            if (connection->sendPipe.getSize() > sendQueueLimit) {
                std::cerr << "Client: Send queue full" << std::endl;

                // Disconnect the client if the send queue is full
                disconnectClient(clientId);
                break;
            }

            connection->sendPipe.push(message);
            break;
        }
    }
}

void Server::handleConnection(std::shared_ptr<ClientConnection> connection) {
    receivePipe.push(connection->id, MagnificentReceivePipe::EventType::Connected);

    byte* header = new byte[4];
    byte* buffer = new byte[maxMessageSize];

    while (connection->client->connected() && running) {
        // Don't burn too much CPU while idling
        std::this_thread::sleep_for(std::chrono::microseconds(1));

        if (connection->client->isWritable()) {
            if (!connection->sendPipe.isEmpty()) {
                std::shared_ptr<ArraySegment<byte>> message;
                if (connection->sendPipe.pop(message)) {
                    NetworkWriter writer;
                    // Write the message size
                    writer.write(message->getSize());
                    // Write the message
                    writer.writeArraySegment(message);

                    std::shared_ptr<ArraySegment<byte>> segment = writer.toArraySegment();
                    connection->client->send(segment->toArray(), segment->getSize());
                }

                // We need to throttle our connection transmission rate
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Throttle
            }
        }

        // Check for incoming data
        if (connection->client->isReadable()) {
            try {
                // Clear the buffer
                std::memset(header, 0, 4);
                std::memset(buffer, 0, maxMessageSize);

                // Read the message size
                int size = 0;
                if (!connection->client->receive(4, header, size)) {
                    // Connection closed
                    break;
                }

                NetworkReader reader(header);
                int messageSize = reader.read<int>();

                if (messageSize <= 0 || messageSize > maxMessageSize) {
                    std::cerr << "Server: Invalid message size Size=" << messageSize << std::endl;
                    break;
                }

                if (!connection->client->receive(messageSize, buffer, size)) {
                    // Connection closed
                    break;
                }

                auto message = ArraySegment<byte>::createArraySegment(buffer, size);

                // Push the message to the receive pipe
                receivePipe.push(connection->id, MagnificentReceivePipe::EventType::Data, message);

                if (receivePipe.getSize() >= receiveQueueLimit) {
                    std::cerr << "Server: Receive pipe is full, dropping messages" << std::endl;
                    break;
                }
            } catch(SocketException& e) {
                std::cerr << "Server: Client Exception=" << e.what() << std::endl;
                break;
            }
        }
    }

    // Free buffer
    delete[] header;
    delete[] buffer;

    receivePipe.push(connection->id, MagnificentReceivePipe::EventType::Disconnected);
}
