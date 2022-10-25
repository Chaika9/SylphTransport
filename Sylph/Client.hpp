#pragma once

#include "UdpClient.hpp"
#include "MagnificentReceivePipe.hpp"
#include "KapMirror/Runtime/ArraySegment.hpp"

namespace KapMirror {
namespace Sylph {
    class Client {
        private:
        volatile bool running;
        bool connected;

        std::shared_ptr<UdpClient> client = nullptr;

        MagnificentReceivePipe receivePipe;

        public:
        Client();
        ~Client();

        void connect(std::string ip, int port);

        void disconnect();

        void send(std::shared_ptr<ArraySegment<byte>> message);

        void tick();

        private:
        void dispose();

        void tickIncoming();
    };
}
}
