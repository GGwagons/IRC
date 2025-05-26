#include "MessageBuffer.hpp"

#include <sys/socket.h>

#include <cstring>

MessageBuffer::MessageBuffer(const int fd) : _currentBufferLen(0), _fd(fd) {
}

MessageBuffer::MessageBuffer(const MessageBuffer &other)
    : _currentBufferLen(other._currentBufferLen), _fd(other._fd) {
    std::memcpy(_buffer, other._buffer, other._currentBufferLen);
}

MessageBuffer::~MessageBuffer() {
}

MessageBuffer &MessageBuffer::operator=(const MessageBuffer &rhs) {
    if (this == &rhs) {
        return *this;
    }

    std::memcpy(_buffer, rhs._buffer, rhs._currentBufferLen);

    _currentBufferLen = rhs._currentBufferLen;
    _fd               = rhs._fd;

    return *this;
}

void MessageBuffer::setFd(int fd) {
    _fd = fd;
}

int MessageBuffer::readFd() {
    // This should never happen in theory
    // if (current_buffer_end_ > kBufferSize) {
    //   return current_buffer_end_;
    // }

    const size_t available_bytes = BUFFER_SIZE - _currentBufferLen;
    if (available_bytes == 0) {
        return _currentBufferLen;
    }

    const int read_bytes =
        recv(_fd, _buffer + _currentBufferLen, available_bytes, 0);
    if (read_bytes < 0) {
        return read_bytes;
    }

    // But in theory, even if there's still a buffered message, `read_fd()` will
    // only be called for incomplete messages. Therefore if there's nothing else
    // to read, this means that we cannot complete the message, so we can just
    // return 0;
    if (read_bytes == 0) {
        return 0;
    }

    _currentBufferLen += read_bytes;

    return _currentBufferLen;
}

std::string MessageBuffer::nextCommand() {
    const size_t delimiter_len = 2;

    if (_currentBufferLen <= delimiter_len) {
        throw MessageBuffer::NoCommandException();
    }

    size_t command_len   = 0;
    bool   found_command = false;
    for (; command_len < _currentBufferLen - delimiter_len + 1;
         command_len += 1) {
        const bool is_delimiter =
            _buffer[command_len] == '\r' && _buffer[command_len + 1] == '\n';
        if (!is_delimiter) continue;

        // Add 2, to take into account the delimiter.
        command_len += 2;
        found_command = true;
        break;
    }

    if (!found_command) {
        if (_currentBufferLen == BUFFER_SIZE) {
            throw MessageBuffer::MalformedCommandException();
        }

        throw MessageBuffer::NoCommandException();
    }

    std::string command = std::string(_buffer, command_len);

    std::memmove(_buffer, _buffer + command_len, BUFFER_SIZE - command_len);
    _currentBufferLen -= command_len;

    return command;
}

const char *MessageBuffer::NoCommandException::what() const throw() {
    return "No command found.";
}

const char *MessageBuffer::MalformedCommandException::what() const throw() {
    return "Malformed command.";
}
