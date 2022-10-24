#include "UdpListener.hpp"

using namespace KapMirror::Sylph;

UdpListener::UdpListener(std::shared_ptr<Address> address) {
    if (address == nullptr) {
        throw std::runtime_error("Address cannot be null");
    }
    socket = Socket::createSocket(address);
}

UdpListener::~UdpListener() {
    socket->close();
}

void UdpListener::close() {
    socket->close();
}

void UdpListener::start() {
    socket->bind();
}

void UdpListener::sendTo(std::shared_ptr<Address> address, byte* buffer, int size) {
    socket->sendTo(buffer, size, address);
}

bool UdpListener::receiveFrom(std::shared_ptr<Address> address, int maxMessageSize, byte* buffer, int& size) {
    size = socket->receiveFrom(buffer, maxMessageSize, address);
    if (size <= 0) {
        return false;
    }
    return false;
}

bool UdpListener::isReadable() const {
    return socket->isReadable();
}

bool UdpListener::isWritable() const {
    return socket->isWritable();
}

void UdpListener::setBlocking(bool blocking) {
    socket->setBlocking(blocking);
}
