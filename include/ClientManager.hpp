#pragma once

#include <netinet/in.h>
#include <sys/poll.h>

#include <map>
#include <string>

#include "Client.hpp"

class Server;

class ClientManager {
   private:
    typedef std::map<int, Client *> ClientMap;

    ClientMap clients_;

    ClientManager(const ClientManager &copy);
    ClientManager &operator=(const ClientManager &copy);

   public:
    ClientManager();
    ~ClientManager();

    void process(Server &server, int fd);

    void acceptClient(int epoll_fd, int server_fd);
    void removeClient(int fd);
    void removeClient(const Client &client);

    Client *getClient(int fd);
    Client *getClientByNick(const std::string &nick);
};
