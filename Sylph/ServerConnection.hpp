#pragma once

#include "Connection.hpp"

namespace KapMirror {
namespace Sylph {
    class ServerConnection : public Connection {
        public:
        ServerConnection(int _connectionId, std::shared_ptr<Address> _address);
    };
}
}
