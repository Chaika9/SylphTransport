#include "Client.hpp"
#include <iostream>

using namespace KapMirror::Sylph;

Client::Client() {
    running = false;
    connected = false;
}

Client::~Client() {
    if (running) {
        dispose();
    }
}

void Client::dispose() {
    running = false;
    connected = false;
    client->close();

    // clean values
    client = nullptr;
}

void Client::connect(std::string ip, int port) {
    if (connected) {
        std::cerr << "Client: Sylph Client can not create connection because an existing connection is connected" << std::endl;
        return;
    }

    if (port < 0 || port > 65535) {
        throw std::runtime_error("Invalid port number");
    }

    running = true;
    connected = true;

    auto address = Address::createAddress(ip, port, KapMirror::Sylph::Address::SocketType::UDP);
    client = std::make_shared<UdpClient>(address);
}

void Client::disconnect() {
}

void Client::send(std::shared_ptr<ArraySegment<byte>> message) {
    client->send(message->toArray(), message->getSize());
}

void Client::tick() {
    tickIncoming();
}

void Client::tickIncoming() {
}
