#include "../include/Client.hpp"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "MessageBuffer.hpp"
#include "numerics.hpp"

Client::Client(int epollFd, int fd, const std::string &ipAddress,
               const unsigned short port)
    : _isAuth(false),
      _login(false),
      _clientSock(fd),
      _epollFd(epollFd),
      _port(port),
      _messageBuffer(fd),
      _ip(ipAddress),
      _nickName(""),
      _realName(""),
      _userName("") {
    struct epoll_event event;
    event.data.fd = fd;
    event.events  = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
        throw std::runtime_error("Failed to add fd to epoll");
    }
}

Client::~Client() {
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, _clientSock, NULL);
    close(_clientSock);
}

int Client::getSocket() const {
    return (_clientSock);
}

std::string Client::getIp() const {
    return (_ip);
}

int Client::getPort() const {
    return (_port);
}

void Client::setUserName(const std::string &userName) {
    _userName = userName;
}
std::string Client::getUserName() const {
    return (_userName);
}

void Client::setNickName(const std::string &nickName) {
    _nickName = nickName;
}
std::string Client::getNickName() const {
    return (_nickName);
}

void Client::setRealName(const std::string &realName) {
    _realName = realName;
}
std::string Client::getRealName() const {
    return (_realName);
}

bool Client::getIsAuth() const {
    return (_isAuth);
}
void Client::setIsAuth() {
    _isAuth = true;
}

bool Client::getLoggedIn() const {
    return _login;
}
void Client::setLoggedIn() {
    _login = true;
}

MessageBuffer &Client::getMessageBuffer() {
    return _messageBuffer;
}

bool Client::ensure_auth() const {
    if (_isAuth) return true;

    std::string reply = "ERROR :You may send PASS before NICK or USER";
    send(reply);

    return false;
}

bool Client::is_registered() const {
    return getLoggedIn();
}

bool Client::ensure_registerd() const {
    if (getLoggedIn()) return true;

    send_numeric_response("localhost", ERR_NOTREGISTERED);

    return false;
}

int Client::send_numeric_response(const std::string &prefix, int code) const {
    std::string message = getMessage(code);

    std::string nick = _nickName;
    if (nick == "") {
        nick = "*";
    }

    std::ostringstream reply;
    reply << ":" << prefix << " " << std::setw(3) << std::setfill('0') << code
          << " " << nick << " :" << message;

    message = reply.str();
    return send(message);
}

int Client::send_detailed_numeric_response(const std::string &prefix,
                                           const std::string &details,
                                           int                code) const {
    std::string message = getMessage(code);

    std::string nick = _nickName;
    if (nick == "") {
        nick = "*";
    }

    std::ostringstream reply;
    reply << ":" << prefix << " " << std::setw(3) << std::setfill('0') << code
          << " " << nick << " " << details << " :" << message;

    message = reply.str();
    return send(message);
}

std::string Client::get_prefix() const {
    std::stringstream prefix;

    prefix << _nickName << "!" << _userName << "@127.0.0.1";

    return prefix.str();
}

ssize_t Client::send(std::string msg) const {
    if (msg.size() > 500) msg = msg.substr(0, 500);

    msg += "\r\n";
    return ::send(_clientSock, msg.c_str(), msg.length(), MSG_NOSIGNAL);
}
