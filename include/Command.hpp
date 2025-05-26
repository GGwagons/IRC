#pragma once

#ifndef COMMAND_HPP_
#    define COMMAND_HPP_

#    include <string>
#    include <vector>

#    include "Server.hpp"

class Command {
   public:
    Command(const std::string &command);

    ~Command();

    void execute(int fd, Server &server);

    const std::vector<std::string> &getComponents() const;

   private:
    std::vector<std::string> _components;

    Command();
    Command(const Command &other);

    Command &operator=(const Command &rhs);

    std::vector<std::string> splitCommand(const std::string &command);
};

#endif
