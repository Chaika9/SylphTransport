#include "UdpClient.hpp"
#include "SocketException.hpp"

using namespace KapMirror::Sylph;

UdpClient::UdpClient(std::shared_ptr<Address> address) {
    if (address == nullptr) {
        throw std::runtime_error("Address cannot be null");
    }
    socket = Socket::createSocket(address);
    isConnected = false;
}

UdpClient::UdpClient(std::shared_ptr<Socket> socket) {
    if (socket == nullptr) {
        throw std::runtime_error("Socket cannot be null");
    }
    this->socket = socket;
    isConnected = true;
}

UdpClient::~UdpClient() {
    socket->close();
}

void UdpClient::close() {
    socket->close();
    isConnected = false;
}

void UdpClient::connect() {
    if (isConnected) {
        throw SocketException("Already connected");
    }
    socket->connect();
    isConnected = true;
}

void UdpClient::send(byte* buffer, int size) {
    if (!isConnected) {
        throw SocketException("Not connected");
    }
    socket->send(buffer, size);
}

bool UdpClient::receive(int maxMessageSize, byte* buffer, int& size) {
    if (!isConnected) {
        throw SocketException("Not connected");
    }
    size = socket->receive(buffer, maxMessageSize);
    if (size <= 0) {
        return false;
    }
    return true;
}

bool UdpClient::isReadable() const {
    if (!isConnected) {
        return false;
    }
    return socket->isReadable();
}

bool UdpClient::isWritable() const {
    if (!isConnected) {
        return false;
    }
    return socket->isWritable();
}

void UdpClient::setBlocking(bool blocking) {
    socket->setBlocking(blocking);
}
