#include <cctype>
#include <string>
#include <vector>

#include "CommandManager.hpp"
#include "Server.hpp"
#include "numerics.hpp"
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

static bool is_valid_nick(const std::string &nick) {
    // <letter>{<letter> | <number> | <special>}
    // <letter>     ::= 'a' ... 'z' | 'A' ... 'Z'
    // <number>     ::= '0' ... '9'
    // <special>    ::= '-' | '[' | ']' | '\' | '`' | '^' | '{' | '}'

    if (nick.empty()) return false;
    if (nick.size() > 50) return false;

    std::string::const_iterator it = nick.begin();
    if (!std::isalpha(*it)) return false;

    for (; it != nick.end(); ++it) {
        if (std::isalpha(*it)) continue;
        if (std::isdigit(*it)) continue;
        if (is_special(*it)) continue;
        if (*it == '-') continue;

        return false;
    }

    return true;
}

void NICK(std::vector<std::string> components, Client &client, Server &server) {
    if (!client.ensure_auth()) {
        server.clientManager().removeClient(client);
        return;
    }

    if (components.size() < 2) {
        client.send_numeric_response("localhost", ERR_NONICKNAMEGIVEN);

        return;
    }

    if (components.size() > 3) {
        client.send_detailed_numeric_response("localhost", "NICK",
                                              ERR_NEEDMOREPARAMS);

        return;
    }

    const bool is_nickname_change = client.getNickName() != "";

    const std::string &nick = get_trailing_value(components[1]);
    if (!is_valid_nick(nick)) {
        client.send_detailed_numeric_response("localhost", nick,
                                              ERR_ERRONEUSNICKNAME);

        return;
    }

    Client *existing_client = server.clientManager().getClientByNick(nick);
    if (existing_client == &client) return;

    if (existing_client != NULL) {
        client.send_detailed_numeric_response("localhost", nick,
                                              ERR_NICKNAMEINUSE);

        return;
    }

    std::string msg = ":" + client.get_prefix() + " NICK :" + nick;
    server.channelManager().broadcastSharedAllUnique(client, msg);

    client.setNickName(nick);

    if (!is_nickname_change && client.getUserName() != "") {
        client.setLoggedIn();
        client.send_numeric_response("localhost", RPL_WELCOME);
    }
}
