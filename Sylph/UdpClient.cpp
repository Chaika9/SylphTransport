#include "UdpClient.hpp"

using namespace KapMirror::Sylph;

UdpClient::UdpClient(const std::shared_ptr<Address>& address) {
    if (address == nullptr) {
        throw std::runtime_error("Address cannot be null");
    }
    socket = Socket::createSocket(address);
}

UdpClient::UdpClient(const std::shared_ptr<Socket>& socket) {
    if (socket == nullptr) {
        throw std::runtime_error("Socket cannot be null");
    }
    this->socket = socket;
}

UdpClient::~UdpClient() { socket->close(); }

void UdpClient::close() { socket->close(); }

void UdpClient::send(byte* buffer, int size) { socket->sendTo(buffer, size, socket->getAddress()); }

bool UdpClient::receive(int maxMessageSize, byte* buffer, int& size) {
    size = socket->receiveFrom(buffer, maxMessageSize, socket->getAddress());
    if (size <= 0) {
        return false;
    }
    return true;
}

bool UdpClient::isReadable() const { return socket->isReadable(); }

bool UdpClient::isWritable() const { return socket->isWritable(); }

void UdpClient::setBlocking(bool blocking) { socket->setBlocking(blocking); }
