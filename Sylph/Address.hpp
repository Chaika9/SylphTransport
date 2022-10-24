#pragma once

#include "Internal.hpp"
#include <memory>
#include <string>

namespace KapMirror {
namespace Sylph {
    class Address {
        public:
        enum class SocketType {
            TCP = SOCK_STREAM,
            UDP = SOCK_DGRAM
        };

        private:
        addrinfo *address = nullptr;

        public:
        Address();
        Address(std::string host, int port, SocketType type = SocketType::TCP);
        Address(int port, bool passive = true, SocketType type = SocketType::TCP);
        ~Address();

        /**
         * @brief Get Address Info
         *
         * @return addrinfo*
         */
        addrinfo* getAddress() const {
            return address;
        }

        static std::shared_ptr<Address> createAddress();

        static std::shared_ptr<Address> createAddress(std::string host, int port, SocketType type = SocketType::TCP);

        static std::shared_ptr<Address> createAddress(int port, bool passive = true, SocketType type = SocketType::TCP);
    };
}
}
