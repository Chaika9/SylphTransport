#include "Socket.hpp"
#include "SocketException.hpp"
#include <stdexcept>
#include <iostream>
#include <cstring>

using namespace KapMirror::Sylph;

Socket::Socket(std::shared_ptr<Address> _address) : address(_address), socket_fd(INVALID_SOCKET) {
    if (address == nullptr) {
        throw std::runtime_error("Address cannot be null");
    }

    addrinfo* addr = address->getAddress();
    if (addr == nullptr) {
        throw std::runtime_error("Address cannot be null");
    }
    socket_fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (socket_fd == INVALID_SOCKET) {
        throw SocketException("Socket creation error");
    }

    int opval = 1;
#ifdef __WINDOWS__
    ::setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opval, sizeof(int));
#else
    ::setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opval, sizeof(int));
#endif
}

Socket::Socket(std::shared_ptr<Address> _address, SOCKET _socket_fd) : address(_address), socket_fd(_socket_fd) {
    if (address == nullptr) {
        throw std::runtime_error("Address cannot be null");
    }

    addrinfo* addr = address->getAddress();
    if (addr == nullptr) {
        throw std::runtime_error("Address cannot be null");
    }
}

Socket::~Socket() {
    close();
}

std::shared_ptr<Address> Socket::getAddress() const {
    return address;
}

void Socket::close() {
    if (socket_fd != INVALID_SOCKET) {
        ::shutdown(socket_fd, SD_SEND);
#ifdef __WINDOWS__
        ::closesocket(socket_fd);
#else
        ::close(socket_fd);
#endif
        socket_fd = INVALID_SOCKET;
    }
}

void Socket::bind() {
    addrinfo* addr = address->getAddress();
    int status = ::bind(socket_fd, addr->ai_addr, addr->ai_addrlen);
    int lastError = SocketLastError;
    if (status == SOCKET_ERROR && lastError != EWOULDBLOCK && lastError != EAGAIN && lastError != EINPROGRESS) {
        throw SocketException("Socket bind error");
    }
}

void Socket::listen() {
    int status = ::listen(socket_fd, SOMAXCONN);
    int lastError = SocketLastError;
    if (status == SOCKET_ERROR && lastError != EWOULDBLOCK && lastError != EAGAIN && lastError != EINPROGRESS) {
        throw SocketException("Socket listen error");
    }
}

void Socket::connect() {
    if (socket_fd == INVALID_SOCKET) {
        throw SocketException("Socket is not initialized");
    }

    addrinfo* addr = address->getAddress();
    int status = ::connect(socket_fd, addr->ai_addr, static_cast<int>(addr->ai_addrlen));
    if (status == 0) {
        return;
    }
    int lastError = SocketLastError;
    if (status == SOCKET_ERROR && lastError != EWOULDBLOCK && lastError != EAGAIN && lastError != EINPROGRESS) {
        throw SocketException("Socket connect error");
    }
}

std::shared_ptr<Socket> Socket::accept() {
    SOCKET client_fd = ::accept(socket_fd, NULL, NULL);
    if (client_fd == INVALID_SOCKET) {
        throw SocketException("Socket accept error");
    }
    return Socket::createSocket(address, client_fd);
}

void Socket::setBlocking(bool blocking) {
    u_long mode = blocking ? 0 : 1;
#ifdef __WINDOWS__
    int status = ioctlsocket(socket_fd, FIONBIO, &mode);
#else
    int status = ioctl(socket_fd, FIONBIO, (char *)&mode);
#endif
    if (status == SOCKET_ERROR) {
        throw SocketException("Socket set blocking error");
    }
}

void Socket::send(byte* buffer, int size, uint32_t flags) {
#ifdef __WINDOWS__
    int status = ::send(socket_fd, (const char *)buffer, size, flags);
#else
    int status = ::send(socket_fd, buffer, size, flags);
#endif
    if (status == SOCKET_ERROR || status <= 0) {
        throw SocketException("Socket send error");
    }
}

void Socket::sendTo(byte* buffer, int size, std::shared_ptr<Address> address, uint32_t flags) {
    addrinfo* addr = address->getAddress();
#ifdef __WINDOWS__
    int status = ::sendto(socket_fd, (const char *)buffer, size, flags, addr->ai_addr, addr->ai_addrlen);
#else
    int status = ::sendto(socket_fd, buffer, size, flags, addr->ai_addr, addr->ai_addrlen);
#endif
    if (status == SOCKET_ERROR || status <= 0) {
        throw SocketException("Socket send error");
    }
}

int Socket::receive(byte* buffer, int size, uint32_t flags) {
#ifdef __WINDOWS__
    auto received = ::recv(socket_fd, (char *)buffer, size, flags);
#else
    auto received = ::recv(socket_fd, buffer, size, flags);
#endif
    if (received == SOCKET_ERROR || received <= 0) {
        return 0;
    }
    return static_cast<int>(received);
}

int Socket::receiveFrom(byte* buffer, int size, std::shared_ptr<Address> address, uint32_t flags) {
    sockaddr addr = {0};
    socklen_t addr_len = sizeof(sockaddr);
#ifdef __WINDOWS__
    auto received = ::recvfrom(socket_fd, (char *)buffer, size, flags, &addr, &addr_len);
#else
    auto received = ::recvfrom(socket_fd, buffer, size, flags, &addr, &addr_len);
#endif
    if (received == SOCKET_ERROR || received <= 0) {
        return 0;
    }
    std::memcpy(address->getAddress()->ai_addr, &addr, addr_len);
    address->getAddress()->ai_addrlen = addr_len;
    return static_cast<int>(received);
}

bool Socket::isInvalid() const {
    if (socket_fd == INVALID_SOCKET) {
        return true;
    }

    fd_set set;
    FD_ZERO(&set);
    FD_SET(socket_fd, &set);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;
    int status = ::select(static_cast<int>(socket_fd + 1), nullptr, nullptr, &set, &timeout);
    if (status == SOCKET_ERROR) {
        return false;
    }
    return (status == 1) ? true : false;
}

bool Socket::isReadable() const {
    if (socket_fd == INVALID_SOCKET) {
        return false;
    }

    fd_set set;
    FD_ZERO(&set);
    FD_SET(socket_fd, &set);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;
    int status = ::select(static_cast<int>(socket_fd + 1), &set, nullptr, nullptr, &timeout);
    if (status == SOCKET_ERROR) {
        return false;
    }
    return (status == 1) ? true : false;
}

bool Socket::isWritable() const {
    if (socket_fd == INVALID_SOCKET) {
        return false;
    }

    fd_set set;
    FD_ZERO(&set);
    FD_SET(socket_fd, &set);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;
    int status = ::select(static_cast<int>(socket_fd + 1), nullptr, &set, nullptr, &timeout);
    if (status == SOCKET_ERROR) {
        return false;
    }
    return (status == 1) ? true : false;
}

std::shared_ptr<Socket> Socket::createSocket(std::shared_ptr<Address> address) {
    return std::make_shared<Socket>(address);
}

std::shared_ptr<Socket> Socket::createSocket(std::shared_ptr<Address> address, SOCKET socket_fd) {
    return std::make_shared<Socket>(address, socket_fd);
}
