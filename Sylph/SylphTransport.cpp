#include "SylphTransport.hpp"
#include <iostream>

using namespace KapMirror;

SylphTransport::SylphTransport() {
}

SylphTransport::~SylphTransport() {
    if (server != nullptr) {
        server->close();
    }
}

/**
 * Client
 */

void SylphTransport::createClient() {
    client = std::make_shared<Sylph::Client>(clientMaxMessageSize);
    client->sendQueueLimit = clientSendQueueLimit;
    client->receiveQueueLimit = clientReceiveQueueLimit;

    client->onConnected = [this]() {
        if (onClientConnected) {
            onClientConnected(*this);
        }
    };
    client->onDisconnected = [this]() {
        if (onClientDisconnected) {
            onClientDisconnected(*this);
        }
    };
    client->onData = [this](std::shared_ptr<ArraySegment<byte>> data) {
        if (onClientDataReceived) {
            onClientDataReceived(*this, data);
        }
    };
}

bool SylphTransport::clientConnected() {
    return client != nullptr && client->connected();
}

void SylphTransport::clientConnect(std::string ip, int port) {
    createClient();
    client->connect(ip, port);
}

void SylphTransport::clientDisconnect() {
    if (client != nullptr) {
        client->disconnect();
    }

    // clean values
    client = nullptr;
}

void SylphTransport::clientSend(std::shared_ptr<ArraySegment<byte>> data) {
    if (client != nullptr) {
        client->send(data);
    }
}

void SylphTransport::clientEarlyUpdate() {
    if (client != nullptr) {
        client->tick(clientMaxReceivesPerTick);
    }
}

/**
 * Server
 */

void SylphTransport::serverStart(int port) {
    // Create server
    server = std::make_shared<Sylph::Server>(serverMaxMessageSize);
    server->sendQueueLimit = serverSendQueueLimitPerConnection;
    server->receiveQueueLimit = serverReceiveQueueLimitPerConnection;

    // Servers Hooks
    server->onConnected = [this](int connectionId) {
        if (onServerConnected) {
            onServerConnected(*this, connectionId);
        }
    };
    server->onDisconnected = [this](int connectionId) {
        if (onServerDisconnected) {
            onServerDisconnected(*this, connectionId);
        }
    };
    server->onData = [this](int connectionId, std::shared_ptr<ArraySegment<byte>> data) {
        if (onServerDataReceived) {
            onServerDataReceived(*this, connectionId, data);
        }
    };

    server->start(port);
}

void SylphTransport::serverStop() {
    if (server) {
        server->close();
    }

    // clean values
    server = nullptr;
}

void SylphTransport::serverSend(int connectionId, std::shared_ptr<ArraySegment<byte>> data) {
    if (server != nullptr) {
        server->send(connectionId, data);
    }
}

void SylphTransport::serverDisconnect(int connectionId) {
    if (server != nullptr) {
        server->disconnectClient(connectionId);
    }
}

void SylphTransport::serverEarlyUpdate() {
    if (server != nullptr) {
        server->tick(serverMaxReceivesPerTick);
    }
}
