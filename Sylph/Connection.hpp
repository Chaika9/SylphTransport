#pragma once

#include "Address.hpp"
#include "KapMirror/Runtime/ArraySegment.hpp"
#include "KapMirror/Runtime/NetworkWriter.hpp"
#include <functional>

namespace KapMirror {
namespace Sylph {
    class Connection {
        protected:
        enum MessageType {
            Handshake = 0,
            Data,
            Disconnect
        };

        enum ConnectionState {
            Disconnected,
            Connected,
            Authenticated
        };

        protected:
        int connectionId;
        std::shared_ptr<Address> address = nullptr;
        ConnectionState state;

        public:
        Connection(int _connectionId, std::shared_ptr<Address> _address);
        virtual ~Connection() = default;

        int getConnectionId() const;

        void disconnect();

        void send(std::shared_ptr<ArraySegment<byte>> message);

        void sendHandshake();

        void rawInput(byte* buffer, int msgLength);

        protected:
        virtual void rawSend(MessageType type, byte* buffer, int msgLength) = 0;

        private:
        void handleOnConnected(MessageType type, std::shared_ptr<ArraySegment<byte>> message);

        void handleOnAuthenticated(MessageType type, std::shared_ptr<ArraySegment<byte>> message);

        void sendDisconnect();

        public:
        std::function<void(Connection&)> onAuthenticated = nullptr;
        std::function<void(Connection&, std::shared_ptr<ArraySegment<byte>>)> onData = nullptr;
        std::function<void(Connection&)> onDisconnected = nullptr;
    };
}
}
