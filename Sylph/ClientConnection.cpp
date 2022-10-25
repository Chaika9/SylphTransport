#include "ClientConnection.hpp"
#include <iostream>

using namespace KapMirror::Sylph;

ClientConnection::ClientConnection(std::shared_ptr<Address> _address, std::shared_ptr<UdpClient> _client) : Connection(0, _address), client(_client) {
}

void ClientConnection::connect() {
    sendHandshake();
}

void ClientConnection::rawSend(MessageType type, byte* buffer, int msgLength) {
    NetworkWriter writer;
    writer.write((byte)type);

    if (buffer != nullptr) {
        writer.writeBytes(buffer, 0, msgLength);
    }

    client->send(writer.toArray(), writer.size());
}
