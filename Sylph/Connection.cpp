#include "Connection.hpp"
#include <iostream>

using namespace KapMirror::Sylph;

Connection::Connection(int _connectionId, std::shared_ptr<Address> _address) : connectionId(_connectionId), address(_address) {
}

void Connection::disconnect() {
}

void Connection::send(std::shared_ptr<ArraySegment<byte>> message) {
}

void Connection::rawInput(byte* buffer, int msgLength) {
    std::cout << "Connection[" << connectionId << "]: Raw input, msgLength=" << msgLength << std::endl;
}
