#pragma once

#ifndef COMMAND_MANAGER_HPP_
#    define COMMAND_MANAGER_HPP_

#    include <map>
#    include <string>
#    include <vector>

#    include "Server.hpp"

class CommandManager {
   public:
    typedef void (*CommandExecutor)(std::vector<std::string> components,
                                    Client &client, Server &server);

    CommandManager();

    ~CommandManager();

    CommandExecutor get(const std::string &command);

   private:
    typedef std::map<std::string, CommandExecutor> CommandMap;

    CommandMap command_map_;

    CommandManager(const CommandManager &other);

    CommandManager &operator=(const CommandManager &rhs);
};

int send_numeric_response(int fd, const std::string &prefix, int code);
int send_detailed_numeric_response(int fd, const std::string &prefix,
                                   const std::string &details, int code);

void INVITE(std::vector<std::string> components, Client &client,
            Server &server);
void JOIN(std::vector<std::string> components, Client &client, Server &server);
void KICK(std::vector<std::string> components, Client &client, Server &server);
void MODE(std::vector<std::string> components, Client &client, Server &server);
void NICK(std::vector<std::string> components, Client &client, Server &server);
void PART(std::vector<std::string> components, Client &client, Server &server);
void PASS(std::vector<std::string> components, Client &client, Server &server);
void PING(std::vector<std::string> components, Client &client, Server &server);
void PRIVMSG(std::vector<std::string> components, Client &client,
             Server &server);
void QUIT(std::vector<std::string> components, Client &client, Server &server);
void TOPIC(std::vector<std::string> components, Client &client, Server &server);
void USER(std::vector<std::string> components, Client &client, Server &server);

#endif
