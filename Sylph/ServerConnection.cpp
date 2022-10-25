#include "ServerConnection.hpp"
#include <iostream>

using namespace KapMirror::Sylph;

ServerConnection::ServerConnection(int _connectionId, std::shared_ptr<Address> _address, std::shared_ptr<UdpListener> _listener) : Connection(_connectionId, _address), listener(_listener) {
}

void ServerConnection::rawSend(MessageType type, byte* buffer, int msgLength) {
    NetworkWriter writer;
    writer.write((byte)type);

    if (buffer != nullptr) {
        writer.writeBytes(buffer, 0, msgLength);
    }

    listener->sendTo(address, writer.toArray(), writer.size());
}
