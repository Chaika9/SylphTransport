#pragma once

#include "KapMirror/Runtime/Transport.hpp"
#include "Client.hpp"
#include "Server.hpp"

namespace KapMirror {
    class SylphTransport : public Transport {
        private:
        std::shared_ptr<Sylph::Client> client;
        std::shared_ptr<Sylph::Server> server;

        public:
        SylphTransport();
        ~SylphTransport();

        bool clientConnected() override;

        void clientConnect(std::string ip, int port) override;

        void clientDisconnect() override;

        void clientSend(std::shared_ptr<ArraySegment<byte>> data) override;

        void clientEarlyUpdate() override;

        void serverStart(int port) override;

        void serverStop() override;

        void serverSend(int connectionId, std::shared_ptr<ArraySegment<byte>> data) override;

        void serverDisconnect(int connectionId) override;

        void serverEarlyUpdate() override;

        private:
        void createClient();
    };
}
