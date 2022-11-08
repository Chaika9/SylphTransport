#include "SylphTransport.hpp"
#include <iostream>

using namespace KapMirror;

SylphTransport::~SylphTransport() {
    if (server != nullptr) {
        server->close();
    }
}

#pragma region Client

void SylphTransport::createClient() {
    client = std::make_shared<Sylph::Client>();

    client->onConnected = [this](Sylph::Client&) {
        if (onClientConnected) {
            onClientConnected(*this);
        }
    };
    client->onDisconnected = [this](Sylph::Client&) {
        if (onClientDisconnected) {
            onClientDisconnected(*this);
        }
    };
    client->onData = [this](Sylph::Client&, std::shared_ptr<ArraySegment<byte>> data) {
        if (onClientDataReceived) {
            onClientDataReceived(*this, data);
        }
    };
}

bool SylphTransport::clientConnected() { return client != nullptr && client->isConnected(); }

void SylphTransport::clientConnect(const std::string& ip, int port) {
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
        client->tick();
    }
}

#pragma endregion

#pragma region Server

void SylphTransport::serverStart(int port) {
    // Create server
    server = std::make_shared<Sylph::Server>();

    // Servers Hooks
    server->onConnected = [this](Sylph::Server&, int connectionId) {
        if (onServerConnected) {
            onServerConnected(*this, connectionId);
        }
    };
    server->onDisconnected = [this](Sylph::Server&, int connectionId) {
        if (onServerDisconnected) {
            onServerDisconnected(*this, connectionId);
        }
    };
    server->onData = [this](Sylph::Server&, int connectionId, std::shared_ptr<ArraySegment<byte>> data) {
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
        server->disconnect(connectionId);
    }
}

void SylphTransport::serverEarlyUpdate() {
    if (server != nullptr) {
        server->tick();
    }
}

#pragma endregion
