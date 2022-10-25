#pragma once

#include "Connection.hpp"
#include "UdpClient.hpp"

namespace KapMirror {
namespace Sylph {
    class ClientConnection : public Connection {
        private:
        std::shared_ptr<UdpClient> client;

        public:
        ClientConnection(std::shared_ptr<Address> _address, std::shared_ptr<UdpClient> _client);

        void connect();

        protected:
        void rawSend(MessageType type, byte* buffer, int msgLength) override;
    };
}
}
