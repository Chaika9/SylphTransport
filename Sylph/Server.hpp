#pragma once

#include "KapMirror/Core/Network/UdpListener.hpp"
#include "KapMirror/Core/ArraySegment.hpp"
#include "ServerConnection.hpp"
#include "Dictionary.hpp"
#include <functional>
#include <list>

#define MTU_DEF                                                                                                                            \
    1200 // default MTU (reduced to 1200 to fit all cases: https://en.wikipedia.org/wiki/Maximum_transmission_unit ; steam uses 1200 too!)

namespace KapMirror::Sylph {
    class Server {
      private:
        std::shared_ptr<UdpListener> listener = nullptr;

        KapEngine::Dictionary<int, std::shared_ptr<ServerConnection>> connections;
        std::list<int> connectionsToRemove;

        byte rawReceiveBuffer[MTU_DEF];

      public:
        Server() = default;
        ~Server();

        void close();

        void start(int port);

        void disconnect(int connectionId);

        void send(int connectionId, const std::shared_ptr<ArraySegment<byte>>& message);

        void tick();

      private:
        void tickIncoming();

        int getConnectionId(const std::shared_ptr<Address>& address);

      public:
        std::function<void(Server&, int)> onConnected = nullptr;
        std::function<void(Server&, int)> onDisconnected = nullptr;
        std::function<void(Server&, int, const std::shared_ptr<ArraySegment<byte>>&)> onData = nullptr;
    };
} // namespace KapMirror::Sylph
