#include "ServerConnection.hpp"

using namespace KapMirror::Sylph;

ServerConnection::ServerConnection(int _connectionId, std::shared_ptr<Address> _address) : Connection(_connectionId, _address) {
}
