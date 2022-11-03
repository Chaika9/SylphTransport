#pragma once

#include "Socket.hpp"
#include "SocketException.hpp"
#include "KapMirror/Runtime/ArraySegment.hpp"

namespace KapMirror::Sylph {
    class UdpClient {
        private:
        std::shared_ptr<Socket> socket;

        public:
        UdpClient(std::shared_ptr<Address> address);
        UdpClient(std::shared_ptr<Socket> socket);
        ~UdpClient();

        /**
         * @brief Close the connection
         */
        void close();

        /**
         * @brief Send data to the server
         *
         * @param buffer Buffer to send
         * @param size Size of the buffer
         */
        void send(byte* buffer, int size);

        /**
         * @brief Receive data from the server
         *
         * @param buffer Buffer to receive into
         * @param size Size of the buffer
         *
         * @return Connection is closed
         */
        bool receive(int maxMessageSize, byte* buffer, int& size);

        bool isReadable() const;

        bool isWritable() const;

        void setBlocking(bool blocking);
    };
}
