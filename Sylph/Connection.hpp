#pragma once

#include "Address.hpp"
#include "KapMirror/Runtime/ArraySegment.hpp"

namespace KapMirror {
namespace Sylph {
    class Connection {
        protected:
        int connectionId;
        std::shared_ptr<Address> address;

        public:
        Connection(int _connectionId, std::shared_ptr<Address> _address);
        virtual ~Connection() = default;

        void disconnect();

        void send(std::shared_ptr<ArraySegment<byte>> message);

        void rawInput(byte* buffer, int msgLength);
    };
}
}
