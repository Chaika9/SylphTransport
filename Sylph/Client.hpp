#pragma once

#include "UdpClient.hpp"
#include "ClientConnection.hpp"
#include "KapMirror/Runtime/ArraySegment.hpp"
#include <functional>

#define MTU_DEF                                                                                                                            \
    1200 // default MTU (reduced to 1200 to fit all cases: https://en.wikipedia.org/wiki/Maximum_transmission_unit ; steam uses 1200 too!)

namespace KapMirror::Sylph {
    class Client {
      private:
        bool connected;

        std::shared_ptr<UdpClient> client            = nullptr;
        std::shared_ptr<ClientConnection> connection = nullptr;

        byte rawReceiveBuffer[MTU_DEF] = {0};

      public:
        Client();
        ~Client();

        bool isConnected() const;

        void connect(std::string ip, int port);

        void disconnect();

        void send(std::shared_ptr<ArraySegment<byte>> message);

        void tick();

      private:
        void dispose();

        void tickIncoming();

      public:
        std::function<void(Client&)> onConnected                                 = nullptr;
        std::function<void(Client&)> onDisconnected                              = nullptr;
        std::function<void(Client&, std::shared_ptr<ArraySegment<byte>>)> onData = nullptr;
    };
} // namespace KapMirror::Sylph
