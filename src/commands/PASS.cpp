#include <iostream>
#include <string>
#include <vector>

#include "CommandManager.hpp"
#include "Server.hpp"
#include "numerics.hpp"
#include "utils.hpp"

void PASS(std::vector<std::string> components, Client &client, Server &server) {
    if (components.size() != 2) {
        client.send_detailed_numeric_response("localhost", "PASS",
                                              ERR_NEEDMOREPARAMS);

        server.clientManager().removeClient(client.getSocket());

        return;
    }

    if (client.getIsAuth()) {
        client.send_numeric_response("localhost", ERR_ALREADYREGISTERED);

        return;
    }

    if (get_trailing_value(components[1]) != server.getPass()) {
        client.send_numeric_response("localhost", ERR_PASSWDMISMATCH);
        std::string reply = "ERROR :Password incorrect";
        client.send(reply);

        server.clientManager().removeClient(client.getSocket());

        return;
    }

    client.setIsAuth();
}
