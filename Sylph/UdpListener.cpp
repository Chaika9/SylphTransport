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
    socket->listen();
}

std::shared_ptr<UdpClient> UdpListener::acceptUdpClient() {
    auto clientSocket = socket->accept();
    if (clientSocket->isInvalid()) {
        throw SocketException("Invalid socket");
    }
    return std::make_shared<UdpClient>(clientSocket);
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
