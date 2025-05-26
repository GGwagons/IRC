#include "ClientManager.hpp"

#include <arpa/inet.h>
#include <fcntl.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Command.hpp"
#include "CommandManager.hpp"
#include "MessageBuffer.hpp"
#include "numerics.hpp"

ClientManager::ClientManager() {
}

ClientManager::~ClientManager() {
    for (ClientMap::iterator it = clients_.begin(); it != clients_.end();
         ++it) {
        delete it->second;
    }
}

void ClientManager::process(Server &server, int fd) {
    Client *client = getClient(fd);
    if (client == NULL) {
        return;
    }

    size_t bytes_received = client->getMessageBuffer().readFd();
    if (bytes_received <= 0) {
        std::vector<std::string> args;
        args.push_back("QUIT");
        args.push_back(":Connection reset by peer");
        QUIT(args, *client, server);

        return;
    }

    while (true) {
        try {
            // Commands like `QUIT` can disconnect the client, therefore we
            // should not process anything afterwards.
            if (getClient(fd) == NULL) return;

            std::string msg = client->getMessageBuffer().nextCommand();
            Command     cmd(msg);
            cmd.execute(fd, server);
        } catch (const MessageBuffer::NoCommandException &) {
            return;
        } catch (const MessageBuffer::MalformedCommandException &e) {
            client->send_numeric_response("localhost", ERR_INPUTTOOLONG);
            std::vector<std::string> args;
            args.push_back("QUIT");
            args.push_back(":Excess flood");
            QUIT(args, *client, server);

            break;
        } catch (...) {
            throw;
        }
    }
}

void ClientManager::acceptClient(int epoll_fd, int server_fd) {
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));

    socklen_t client_len = sizeof(clientAddress);
    int       client_socket =
        accept(server_fd, (struct sockaddr *)&(clientAddress), &client_len);
    if (client_socket == -1) {
        throw std::runtime_error("Failed to accept");
    }
    if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1) {
        throw std::runtime_error("Failed to set non-blocking");
    }

    Client *client =
        new Client(epoll_fd, client_socket, inet_ntoa(clientAddress.sin_addr),
                   ntohs(clientAddress.sin_port));

    try {
        clients_[client_socket] = client;
    } catch (...) {
        delete client;

        throw;
    }
}

void ClientManager::removeClient(int fd) {
    Client *client = getClient(fd);
    if (client == NULL) {
        return;
    }

    clients_.erase(fd);

    delete client;
}

void ClientManager::removeClient(const Client &client) {
    removeClient(client.getSocket());
}

Client *ClientManager::getClient(int fd) {
    if (clients_.find(fd) == clients_.end()) return NULL;

    return clients_[fd];
}

Client *ClientManager::getClientByNick(const std::string &nick) {
    for (ClientManager::ClientMap::const_iterator it = clients_.begin();
         it != clients_.end(); ++it) {
        if (it->second->getNickName() != nick) continue;

        return it->second;
    }

    return NULL;
}
