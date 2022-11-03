#pragma once

#include <exception>
#include <string>

namespace KapMirror::Sylph {
    class SocketException : public std::exception {
        std::string _message;

    public:
        SocketException(std::string const &message) : _message(message) {}
        ~SocketException() = default;

        char const *what() const noexcept {
            return _message.c_str();
        }
    };
}
