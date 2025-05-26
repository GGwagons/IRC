#include <csignal>
#include <iostream>

#include "../include/Server.hpp"
#include "utils.hpp"

static bool is_special(const char c) {
    switch (c) {
        case '[':
        case ']':
        case '\\':
        case '`':
        case '_':
        case '^':
        case '{':
        case '|':
        case '}':
            return true;

        default:
            return false;
    }
}

static bool is_valid_pass(const std::string &pass) {
    if (pass.empty()) return false;
    if (pass.size() > 32) return false;

    std::string::const_iterator it = pass.begin();
    if (!std::isalpha(*it)) return false;

    for (; it != pass.end(); ++it) {
        if (std::isalpha(*it)) continue;
        if (std::isdigit(*it)) continue;
        if (is_special(*it)) continue;
        if (*it == '-') continue;

        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return (1);
    }

    int port = parse_port(argv[1]);
    if (port == -1) {
        std::cerr << "Invalid port: " << argv[1] << std::endl;
        return 1;
    }

    if (!is_valid_pass(argv[2])) {
        std::cerr << "Invalid password: " << argv[2] << std::endl;
        return 1;
    }

    try {
        Server server;
        signal(SIGINT, sigHandler);
        signal(SIGQUIT, sigHandler);
        server.setPort(port);
        server.setPass(argv[2]);
        server.setSock();
        server.servSetUp();
        server.run();
    } catch (std::exception &e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return (1);
    }
    return (0);
}
