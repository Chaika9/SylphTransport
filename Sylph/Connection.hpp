#pragma once

#include "Address.hpp"
#include "KapMirror/Runtime/ArraySegment.hpp"
#include "KapMirror/Runtime/NetworkWriter.hpp"
#include <functional>

#define PING_INTERVAL 1000
#define DEFAULT_TIMEOUT 10000

namespace KapMirror::Sylph {
    class Connection {
      protected:
        enum MessageType { Handshake = 0, Ping, Data, Disconnect };

        enum ConnectionState { Disconnected, Connected, Authenticated };

      protected:
        int connectionId;
        std::shared_ptr<Address> address = nullptr;
        ConnectionState state;

        int timeout;

        long long lastPingTime    = 0;
        long long lastReceiveTime = 0;

      public:
        Connection(int _connectionId, std::shared_ptr<Address> _address, int _timeout = DEFAULT_TIMEOUT);
        virtual ~Connection() = default;

        int getConnectionId() const;

        void disconnect();

        void send(std::shared_ptr<ArraySegment<byte>> message);

        void sendHandshake();

        void rawInput(byte* buffer, int msgLength);

        void tick();

      protected:
        virtual void rawSend(MessageType type, byte* buffer, int msgLength) = 0;

      private:
        void handleOnConnected(MessageType type, std::shared_ptr<ArraySegment<byte>> message);

        void handleOnAuthenticated(MessageType type, std::shared_ptr<ArraySegment<byte>> message);

        void handlePing(long long time);

        void handleTimeout(long long time);

        void sendDisconnect();

        void sendPing();

      public:
        std::function<void(Connection&)> onAuthenticated                             = nullptr;
        std::function<void(Connection&, std::shared_ptr<ArraySegment<byte>>)> onData = nullptr;
        std::function<void(Connection&)> onDisconnected                              = nullptr;
    };
} // namespace KapMirror::Sylph
