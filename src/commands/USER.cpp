#include <string>
#include <vector>

#include "CommandManager.hpp"
#include "Server.hpp"
#include "numerics.hpp"
#include "utils.hpp"

void USER(std::vector<std::string> components, Client &client, Server &server) {
    if (!client.ensure_auth()) {
        server.clientManager().removeClient(client);
        return;
    }

    if (client.getLoggedIn() || client.getUserName() != "") {
        client.send_numeric_response("localhost", ERR_ALREADYREGISTERED);

        return;
    }

    if (components.size() != 5) {
        client.send_detailed_numeric_response("localhost", "USER",
                                              ERR_NEEDMOREPARAMS);

        return;
    }

    std::string &username = components[1];
    std::string &realname = components[4];

    client.setUserName(username.substr(0, 10));
    client.setRealName(get_trailing_value(realname).substr(0, 50));

    if (client.getNickName() != "") {
        client.setLoggedIn();
        client.send_numeric_response("localhost", RPL_WELCOME);
    }
}
