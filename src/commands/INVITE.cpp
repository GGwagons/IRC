#include <string>
#include <vector>

#include "Channel.hpp"
#include "Server.hpp"
#include "numerics.hpp"
#include "utils.hpp"

void INVITE(std::vector<std::string> components, Client &client,
            Server &server) {
    if (!client.ensure_registerd()) return;

    if (components.size() != 3) {
        client.send_detailed_numeric_response("localhost", "INVITE",
                                              ERR_NEEDMOREPARAMS);

        return;
    }

    const std::string &target_nick  = get_trailing_value(components[1]);
    const std::string &channel_name = get_trailing_value(components[2]);

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

    if (channel->inviteOnlyMode() && !channel->isOperator(client)) {
        client.send_detailed_numeric_response("localhost", channel_name,
                                              ERR_CHANOPRIVSNEEDED);
        return;
    }

    const Client *target = server.clientManager().getClientByNick(target_nick);
    if (target != NULL && channel->hasClient(*target)) {
        client.send_detailed_numeric_response(
            "localhost", target->getNickName() + " " + channel_name,
            ERR_USERONCHANNEL);
        return;
    }

    channel->inviteUser(target_nick);

    if (target != NULL) {
        std::string invite = ":" + client.get_prefix() + " INVITE " +
                             target_nick + " " + channel_name;
        target->send(invite);
    }

    std::string reply = ":localhost 341 " + client.getNickName() + " " +
                        target_nick + " " + channel_name;
    client.send(reply);
}
