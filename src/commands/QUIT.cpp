#include <set>
#include <string>
#include <vector>

#include "Server.hpp"
#include "utils.hpp"

void QUIT(std::vector<std::string> components, Client &client, Server &server) {
    std::string msg = "Quit: ";
    if (components.size() > 1) msg += get_trailing_value(components[1]);

    client.send("ERROR " + msg);

    if (!client.ensure_registerd()) {
        server.clientManager().removeClient(client);

        return;
    }

    std::set<const Client *> already_received;

    msg = ":" + client.get_prefix() + " QUIT :" + msg;
    server.channelManager().broadcastSharedUnique(client, msg);

    server.channelManager().removeUserFromAllChannels(client);

    server.clientManager().removeClient(client);
}
