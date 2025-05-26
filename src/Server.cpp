#include "Server.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "ChannelManager.hpp"
#include "ClientManager.hpp"

bool g_signal = false;

//--------------------------------------------------CONSTRUCTORS--------------------------------------------------

Server::Server() {
    _servSock = -1;
    _epollFd  = epoll_create1(0);
    if (_epollFd == -1) {
        throw new std::runtime_error("Failed to create epoll instance");
    }
}

Server::Server(const Server &copy) {
    *this = copy;
}

Server &Server::operator=(const Server &copy) {
    if (this != &copy) {
        _serverPass = copy._serverPass;
        _port       = copy._port;
        _servSock   = copy._servSock;
        _epollFd    = copy._epollFd;
    }
    return (*this);
}

Server::~Server() {
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, _servSock, NULL);
    close(_servSock);
    close(_epollFd);
}

int Server::getPort() const {
    return (_port);
}
void Server::setPort(int port) {
    _port = port;
}
void Server::setPass(const std::string &pass) {
    _serverPass = pass;
}
const std::string &Server::getPass() const {
    return _serverPass;
}
ClientManager &Server::clientManager() {
    return _clientManager;
}
ChannelManager &Server::channelManager() {
    return _channelManager;
}

int Server::getSock() const {
    return (_servSock);
}
void Server::setSock() {
    _servSock = socket(AF_INET, SOCK_STREAM, 0);
    if (_servSock == -1) {
        throw std::runtime_error("Failed to create socket");
    }
}

//--------------------------------------------------SERVER
// SETUP--------------------------------------------------

void Server::servSetUp() {
    int opt = 1;
    memset(&_serverAddress, 0, sizeof(_serverAddress));
    _serverAddress.sin_family      = AF_INET;
    _serverAddress.sin_port        = htons(_port);
    _serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (setsockopt(_servSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
        -1) {
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }
    if (fcntl(_servSock, F_SETFL, O_NONBLOCK) == -1) {
        throw std::runtime_error("Failed to set non-blocking");
    }
    if (bind(_servSock, (struct sockaddr *)&_serverAddress,
             sizeof(_serverAddress)) == -1) {
        throw std::runtime_error("Failed to bind");
    }
    if (listen(getSock(), SOMAXCONN) == -1) {
        throw std::runtime_error("Failed to listen");
    }

    struct epoll_event event;
    event.data.fd = _servSock;
    event.events  = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _servSock, &event) == -1) {
        throw std::runtime_error("Failed to add fd to epoll");
    }
}

#define MAX_EVENTS 128
void process(Server &server, int epoll_fd) {
    struct epoll_event events[MAX_EVENTS];
    int                count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (count == -1) {
        if (g_signal) {
            return;
        }

        throw new std::runtime_error("epoll wait failed");
    }

    for (int i = 0; i < count; ++i) {
        struct epoll_event &event = events[i];

        if (event.data.fd == server.getSock()) {
            try {
                server.clientManager().acceptClient(epoll_fd, event.data.fd);
            } catch (std::exception &e) {
                std::cerr << "Failed to accept client: " << e.what()
                          << std::endl;
            } catch (...) {
                throw;
            }

            continue;
        }

        try {
            server.clientManager().process(server, event.data.fd);
        } catch (...) {
            throw;
        }
    }
}

void Server::run() {
    std::cout << "Server is running" << std::endl;
    std::cout << "Waiting for clients to connect" << std::endl;
    while (!g_signal) {
        process(*this, _epollFd);
    }
}

//--------------------------------------------------CLEANUP--------------------------------------------------

void sigHandler(int signum) {
    std::cout << std::endl;
    if (signum == SIGINT || signum == SIGQUIT) {
        std::cout << "Signal " << signum << " received. Shutting down..."
                  << std::endl;
        g_signal = true;
    }
}
