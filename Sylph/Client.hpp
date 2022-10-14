#pragma once

#include "UdpClient.hpp"
#include "MagnificentReceivePipe.hpp"
#include "MagnificentSendPipe.hpp"
#include <thread>
#include <mutex>
#include <memory>
#include <functional>

namespace KapMirror {
namespace Sylph {
    class Client {
        private:
        volatile bool running;
        volatile bool isConnecting;

        int maxMessageSize;

        std::shared_ptr<UdpClient> client;
        std::thread clientThread;

        MagnificentReceivePipe receivePipe;
        MagnificentSendPipe sendPipe;

        public:
        int sendQueueLimit = 10000;
        int receiveQueueLimit = 10000;

        public:
        Client(int _maxMessageSize = 1024);
        ~Client();

        void connect(std::string ip, int port);

        void disconnect();

        bool connecting() const {
            return isConnecting;
        }

        bool connected() const {
            return client != nullptr && client->connected();
        }

        int tick(int processLimit);

        void send(std::shared_ptr<ArraySegment<byte>> message);

        private:
        void dispose();

        void run(std::string ip, int port);

        public:
        std::function<void()> onConnected;
        std::function<void()> onDisconnected;
        std::function<void(std::shared_ptr<ArraySegment<byte>>)> onData;
    };
}
}
