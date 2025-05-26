#pragma once

#include "ChannelManager.hpp"
#include "ClientManager.hpp"

class Client;
class Channel;
class ChannelManager;
class Client;
class Channel;
class ClientManager;

class Server {
   private:
    struct sockaddr_in _serverAddress;
    struct sockaddr_in _clientAddress;
    struct pollfd      _newClient;
    int                _port;
    int                _servSock;
    std::string        _serverPass;
    int                _epollFd;

    ClientManager  _clientManager;
    ChannelManager _channelManager;

   public:
    Server();
    Server(const Server &copy);
    Server &operator=(const Server &copy);
    ~Server();

    // Server setup and execution
    void servSetUp();
    void run();

    // Getters and setters
    void               setPort(int port);
    int                getPort() const;
    void               setPass(const std::string &pass);
    const std::string &getPass() const;
    void               setSock();
    int                getSock() const;

    ClientManager  &clientManager();
    ChannelManager &channelManager();

    // Cleanup
    void closeFds();
};

void sigHandler(int signum);
