#pragma once

#include "UdpClient.hpp"
#include "SocketException.hpp"

namespace KapMirror::Sylph {
    class UdpListener {
      private:
        std::shared_ptr<Socket> socket;

      public:
        explicit UdpListener(const std::shared_ptr<Address>& address);
        ~UdpListener();

        /**
         * @brief Close the listener
         */
        void close();

        /**
         * @brief Start listening for connections
         */
        void start();

        void sendTo(const std::shared_ptr<Address>& address, byte* buffer, int size);

        bool receiveFrom(const std::shared_ptr<Address>& address, int maxMessageSize, byte* buffer, int& size);

        bool isReadable() const;

        bool isWritable() const;

        void setBlocking(bool blocking);
    };
} // namespace KapMirror::Sylph
