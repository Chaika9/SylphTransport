#pragma once

#include "Connection.hpp"
#include "KapMirror/Core/Network/UdpListener.hpp"

namespace KapMirror::Sylph {
    class ServerConnection : public Connection {
      private:
        std::shared_ptr<UdpListener> listener;

      public:
        ServerConnection(int _connectionId, std::shared_ptr<Address> _address, std::shared_ptr<UdpListener> _listener);

      protected:
        void rawSend(MessageType type, byte* buffer, int msgLength) override;
    };
} // namespace KapMirror::Sylph
