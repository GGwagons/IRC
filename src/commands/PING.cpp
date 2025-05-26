#include <string>
#include <vector>

#include "CommandManager.hpp"
#include "Server.hpp"
#include "numerics.hpp"
#include "utils.hpp"

void PING(std::vector<std::string> components, Client &client, Server &) {
    if (components.size() != 2) {
        client.send_detailed_numeric_response("localhost", "PING",
                                              ERR_NEEDMOREPARAMS);
        return;
    }

    std::string msg = "PONG localhost " + get_trailing_value(components[1]);
    client.send(msg);
}
