#pragma once

#include "Internal.hpp"
#include "Address.hpp"
#include "KapMirror/Core/ArraySegment.hpp"
#include <memory>
#include <string>

namespace KapMirror::Sylph {
    class Socket {
      private:
        std::shared_ptr<Address> address;
        int socket_fd;

      public:
        explicit Socket(std::shared_ptr<Address> _address);
        Socket(std::shared_ptr<Address> _address, SOCKET _socket_fd);
        ~Socket();

        std::shared_ptr<Address> getAddress() const;

        /**
         * @brief Close the socket
         */
        void close();

        /**
         * @brief Bind the socket
         */
        void bind();

        /**
         * @brief Listen for incoming connections
         */
        void listen() const;

        /**
         * @brief Connect to remote
         */
        void connect();

        /**
         * @brief Accept an incoming connection
         *
         * @return std::shared_ptr<Socket> The accepted socket
         */
        std::shared_ptr<Socket> accept();

        void setBlocking(bool blocking) const;

        /**
         * @brief Send data to remote
         *
         * @param data The data to send
         */
        void send(byte* buffer, int size, uint32_t flags = 0) const;

        /**
         * @brief Send data to remote
         *
         * @param data The data to receive
         * @param address The address to send to
         */
        void sendTo(byte* buffer, int size, const std::shared_ptr<Address>& address, uint32_t flags = 0) const;

        /**
         * @brief Receive data from remote
         *
         * @param buffer The buffer to receive data into
         * @param size The size of the buffer
         *
         * @return int The number of bytes received
         */
        int receive(byte* buffer, int size, uint32_t flags = 0) const;

        /**
         * @brief Receive data from remote
         *
         * @param buffer The buffer to receive data into
         * @param size The size of the buffer
         * @param address The address to receive from
         *
         * @return int The number of bytes received
         */
        int receiveFrom(byte* buffer, int size, const std::shared_ptr<Address>& address, uint32_t flags = 0) const;

        bool isInvalid() const;

        bool isReadable() const;

        bool isWritable() const;

        static std::shared_ptr<Socket> createSocket(const std::shared_ptr<Address>& address);

        static std::shared_ptr<Socket> createSocket(const std::shared_ptr<Address>& address, SOCKET socket_fd);
    };
} // namespace KapMirror::Sylph
