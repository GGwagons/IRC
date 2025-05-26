#include "Command.hpp"

#include <iostream>

#include "CommandManager.hpp"

Command::Command(const std::string &command) {
    _components = Command::splitCommand(command);
}

Command::~Command() {
}

void Command::execute(int fd, Server &server) {
    if (_components.empty()) return;

    size_t     i          = 0;
    const bool has_prefix = _components[i].rfind(":", 0) != std::string::npos;
    if (has_prefix) {
        i += 1;
    }

    std::string                    &command_name = _components[i];
    CommandManager::CommandExecutor exec = CommandManager().get(command_name);
    if (exec == NULL) return;

    Client *client = server.clientManager().getClient(fd);
    if (client == NULL) {
        return;
    }

    exec(_components, *client, server);
}

const std::vector<std::string> &Command::getComponents() const {
    return _components;
}

std::vector<std::string> Command::splitCommand(const std::string &command) {
    std::vector<std::string> components;

    bool   in_word    = false;
    size_t word_start = 0;
    for (size_t i = 0; i < command.length(); i += 1) {
        // Hit the delimiter, which we can ignore.
        if (command[i] == '\r' || command[i] == '\n') {
            if (in_word) {
                components.push_back(
                    command.substr(word_start, i - word_start));
            }

            break;
        }

        // In a command everything which comes after a `:` is treated as a
        // single component. Exception: If the command starts with a prefix,
        // then this prefix starts with a `:`.
        if (!in_word && components.size() >= 1 && command[i] == ':') {
            // // `+1` to ignore the prefix.
            // i += 1;
            if (command[i] == '\r' || command[i] == '\n') {
                break;
            }
            // `command.length() - i - 2` to ignore the `crlf` at the end
            components.push_back(command.substr(i, command.length() - i - 2));

            break;
        }

        if (!in_word && command[i] != ' ') {
            in_word    = true;
            word_start = i;

            continue;
        }

        if (in_word && command[i] == ' ') {
            components.push_back(command.substr(word_start, i - word_start));

            in_word = false;
        }
    }

    return components;
}
