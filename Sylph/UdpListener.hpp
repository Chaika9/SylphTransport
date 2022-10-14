#pragma once

#include "UdpClient.hpp"
#include "SocketException.hpp"

namespace KapMirror {
namespace Sylph {
    class UdpListener {
        private:
        std::shared_ptr<Socket> socket;

        public:
        UdpListener(std::shared_ptr<Address> address);
        ~UdpListener();

        /**
         * @brief Close the listener
         */
        void close();

        /**
         * @brief Start listening for connections
         */
        void start();

        /**
         * @brief Accept a connection
         *
         * @return The client socket
         */
        std::shared_ptr<UdpClient> acceptUdpClient();

        bool isReadable() const;

        bool isWritable() const;

        void setBlocking(bool blocking);
    };
}
}
