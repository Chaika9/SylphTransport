#include "Address.hpp"
#include <iostream>

using namespace KapMirror::Sylph;

Address::Address(std::string host, int port) {
    addrinfo hints = {0};
    hints.ai_family = AF_INET; // Note: Windows fails to connect if left unspecified
    hints.ai_socktype = SOCK_DGRAM; // UDP
    hints.ai_protocol = IPPROTO_UDP; // Set UDP protocol

    int status = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &address);
    if (status != 0) {
        throw std::runtime_error("getaddrinfo failed");
    }
}

Address::Address(int port, bool passive) {
    addrinfo hints = {0};
    hints.ai_family = AF_INET; // Note: Windows fails to connect if left unspecified
    hints.ai_socktype = SOCK_DGRAM; // UDP
    if (passive) {
        hints.ai_flags = AI_PASSIVE;
    }
    hints.ai_protocol = IPPROTO_UDP; // Set UDP protocol

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

std::shared_ptr<Address> Address::createAddress(std::string host, int port) {
    return std::make_shared<Address>(host, port);
}

std::shared_ptr<Address> Address::createAddress(int port, bool passive) {
    return std::make_shared<Address>(port, passive);
}
