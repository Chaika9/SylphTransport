#include "Address.hpp"
#include <iostream>
#include <cstring>

using namespace KapMirror::Sylph;

Address::Address() {
    address = new addrinfo();
    std::memset(address, 0, sizeof(addrinfo));

    address->ai_addr = new sockaddr();
    std::memset(address->ai_addr, 0, sizeof(sockaddr));
}

Address::Address(std::string host, int port, SocketType type) {
    addrinfo hints = {0};
    hints.ai_family = AF_INET; // Note: Windows fails to connect if left unspecified
    hints.ai_socktype = (int)type;

    int status = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &address);
    if (status != 0) {
        throw std::runtime_error("getaddrinfo failed");
    }
}

Address::Address(int port, bool passive, SocketType type) {
    addrinfo hints = {0};
    hints.ai_family = AF_INET; // Note: Windows fails to connect if left unspecified
    hints.ai_socktype = (int)type;
    if (passive) {
        hints.ai_flags = AI_PASSIVE;
    }

    int status = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &address);
    if (status != 0) {
        throw std::runtime_error("getaddrinfo failed");
    }
}

Address::~Address() {
    if (address != nullptr) {
        freeaddrinfo(address);
    }
}

std::shared_ptr<Address> Address::createAddress() {
    return std::make_shared<Address>();
}

std::shared_ptr<Address> Address::createAddress(std::string host, int port, SocketType type) {
    return std::make_shared<Address>(host, port, type);
}

std::shared_ptr<Address> Address::createAddress(int port, bool passive, SocketType type) {
    return std::make_shared<Address>(port, passive, type);
}
