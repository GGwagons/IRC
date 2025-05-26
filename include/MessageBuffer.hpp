#pragma once

#ifndef MESSAGE_BUFFER_HPP_
#    define MESSAGE_BUFFER_HPP_

#    include <string>

#    define BUFFER_SIZE 512

class MessageBuffer {
   public:
    MessageBuffer(const int fd);
    MessageBuffer(const MessageBuffer &other);

    ~MessageBuffer();

    MessageBuffer &operator=(const MessageBuffer &rhs);

    void setFd(int fd);

    int readFd();

    std::string nextCommand();

    class NoCommandException : public std::exception {
        const char *what() const throw();
    };

    class MalformedCommandException : public std::exception {
        const char *what() const throw();
    };

   private:
    char   _buffer[BUFFER_SIZE];
    size_t _currentBufferLen;
    int    _fd;

    MessageBuffer();
};

#endif
