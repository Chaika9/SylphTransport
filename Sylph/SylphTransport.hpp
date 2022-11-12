#pragma once

#include "KapMirror/Core/Transport.hpp"
#include "Client.hpp"
#include "Server.hpp"

namespace KapMirror {
    class SylphTransport : public Transport {
      private:
        std::shared_ptr<Sylph::Client> client;
        std::shared_ptr<Sylph::Server> server;

      public:
        SylphTransport() = default;
        ~SylphTransport() override;

        bool clientConnected() override;

        void clientConnect(const std::string& ip, int port) override;

        void clientDisconnect() override;

        void clientSend(const std::shared_ptr<ArraySegment<byte>>& data) override;

        void clientEarlyUpdate() override;

        void serverStart(int port) override;

        void serverStop() override;

        void serverSend(int connectionId, const std::shared_ptr<ArraySegment<byte>>& data) override;

        void serverDisconnect(int connectionId) override;

        void serverEarlyUpdate() override;

      private:
        void createClient();
    };
} // namespace KapMirror
