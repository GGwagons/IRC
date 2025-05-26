#include <string>
#include <vector>

#include "Channel.hpp"
#include "Server.hpp"
#include "numerics.hpp"
#include "utils.hpp"

void KICK(std::vector<std::string> components, Client &client, Server &server) {
    if (!client.ensure_registerd()) return;

    if (components.size() < 3 || components.size() > 4) {
        client.send_detailed_numeric_response("localhost", "KICK",
                                              ERR_NEEDMOREPARAMS);

        return;
    }

    const std::string &channel_name = components[1];
    if (!Channel::is_valid_channel_name(channel_name)) {
        client.send_detailed_numeric_response("localhost", channel_name,
                                              ERR_BADCHANMASK);
        return;
    }

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

    if (!channel->isOperator(client)) {
        client.send_detailed_numeric_response("localhost", channel_name,
                                              ERR_CHANOPRIVSNEEDED);
        return;
    }

    const std::vector<std::string> users_to_kick =
        split(get_trailing_value(components[2]), ',');
    for (std::vector<std::string>::const_iterator it = users_to_kick.begin();
         it != users_to_kick.end(); ++it) {
        if ((*it).empty()) continue;

        Client *target = server.clientManager().getClientByNick(*it);
        if (target == NULL || !target->is_registered()) {
            std::string msg = *it + " " + channel_name;
            client.send_detailed_numeric_response("localhost", msg,
                                                  ERR_USERNOTINCHANNEL);

            continue;
        }

        std::string reason = "Was kicked from the channel";
        if (components.size() == 4) reason = get_trailing_value(components[3]);

        server.channelManager().kickUser(*channel, client, *target, reason);
    }
}
