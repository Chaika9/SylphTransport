#pragma once

#include "UdpListener.hpp"
#include "ServerConnection.hpp"
#include "MagnificentReceivePipe.hpp"
#include "KapMirror/Runtime/ArraySegment.hpp"
#include "Dictionary.hpp"
#include <thread>
#include <mutex>

#define MTU_DEF 1200 // default MTU (reduced to 1200 to fit all cases: https://en.wikipedia.org/wiki/Maximum_transmission_unit ; steam uses 1200 too!)

namespace KapMirror {
namespace Sylph {
    class Server {
        private:
        std::shared_ptr<UdpListener> listener = nullptr;

        KapEngine::Dictionary<int, std::shared_ptr<ServerConnection>> connections;
        MagnificentReceivePipe receivePipe;

        byte rawReceiveBuffer[MTU_DEF];

        public:
        Server();
        ~Server();

        void close();

        void start(int port);

        void disconnect(int connectionId);

        void send(int connectionId, std::shared_ptr<ArraySegment<byte>> message);

        void tick();

        private:
        void tickIncoming();

        int getConnectionId(std::shared_ptr<Address> address);
    };
}
}
