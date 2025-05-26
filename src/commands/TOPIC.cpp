#include <string>
#include <vector>

#include "Channel.hpp"
#include "Server.hpp"
#include "numerics.hpp"
#include "utils.hpp"

void TOPIC(std::vector<std::string> components, Client &client,
           Server &server) {
    if (!client.ensure_registerd()) return;

    if (components.size() < 2 || components.size() > 3) {
        client.send_detailed_numeric_response("localhost", "TOPIC",
                                              ERR_NEEDMOREPARAMS);
        return;
    }

    const std::string &channel_name = get_trailing_value(components[1]);
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

    if (components.size() == 2) {
        if (!channel->hasTopic()) {
            client.send_detailed_numeric_response("localhost", channel_name,
                                                  RPL_NOTOPIC);
            return;
        }

        std::string reply = ":localhost 332 " + client.getNickName() + " " +
                            channel_name + " :" + channel->getTopic();
        client.send(reply);

        reply = ":localhost 333 " + client.getNickName() + " " +
                channel->getTopicWhotime();
        client.send(reply);

        return;
    }

    if (channel->protectedMode() && !channel->isOperator(client)) {
        client.send_detailed_numeric_response("localhost", channel_name,
                                              ERR_CHANOPRIVSNEEDED);
        return;
    }

    const std::string &topic = get_trailing_value(components[2]);
    channel->setTopic(client, topic);

    std::string info =
        ":" + client.get_prefix() + " TOPIC " + channel_name + " :" + topic;
    channel->broadcastAll(info);
}
