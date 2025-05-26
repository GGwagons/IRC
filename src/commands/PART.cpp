#include "Channel.hpp"
#include "CommandManager.hpp"
#include "numerics.hpp"
#include "utils.hpp"

static void part_channel(Server &server, Client &client,
                         const std::string &channel_name,
                         const std::string &reason) {
    Channel *channel = server.channelManager().getChannel(channel_name);
    if (channel == NULL) {
        client.send_detailed_numeric_response("localhost", channel_name,
                                              ERR_NOSUCHCHANNEL);
        return;
    }

    if (!channel->hasClient(client)) {
        client.send_detailed_numeric_response("localhost", channel_name,
                                              ERR_NOTONCHANNEL);
        return;
    }

    std::string msg =
        ":" + client.get_prefix() + " PART " + channel->getChannelName();
    if (reason != "") {
        msg += " :" + reason;
    }

    channel->broadcastAll(msg);
    server.channelManager().removeUser(*channel, client);
}

void PART(std::vector<std::string> components, Client &client, Server &server) {
    if (!client.ensure_registerd()) return;

    if (components.size() < 2 || components.size() > 3) {
        client.send_detailed_numeric_response("localhost", "JOIN",
                                              ERR_NEEDMOREPARAMS);

        return;
    }

    std::vector<std::string> channels =
        split(get_trailing_value(components[1]), ',');

    std::string reason = "";
    if (components.size() == 3) {
        reason = get_trailing_value(components[2]);
    }

    for (size_t i = 0; i < channels.size(); i++) {
        part_channel(server, client, channels[i], reason);
    }
}
