#pragma once

#include <exception>
#include <string>

namespace KapMirror::Sylph {
    class SocketException : public std::exception {
        std::string _message;

      public:
        explicit SocketException(std::string const& message) : _message(message) {}

        char const* what() const noexcept { return _message.c_str(); }
    };
} // namespace KapMirror::Sylph
