#include <iostream>
#include <string>
#include <vector>

#include "Channel.hpp"
#include "Server.hpp"
#include "numerics.hpp"
#include "utils.hpp"

void join_channel(Server &server, Client &client,
                  const std::string &channel_name, const char *key) {
    if (!Channel::is_valid_channel_name(channel_name)) {
        client.send_detailed_numeric_response("localhost", channel_name,
                                              ERR_BADCHANMASK);
        return;
    }

    server.channelManager().createChannel(&client, channel_name, key);
}

void JOIN(std::vector<std::string> components, Client &client, Server &server) {
    if (!client.ensure_registerd()) return;

    if (components.size() < 2 || components.size() > 3) {
        client.send_detailed_numeric_response("localhost", "JOIN",
                                              ERR_NEEDMOREPARAMS);

        return;
    }

    std::vector<std::string> channels =
        split(get_trailing_value(components[1]), ',');
    std::vector<std::string> keys;

    if (components.size() == 3) {
        keys = split(get_trailing_value(components[2]), ',');
    }

    std::vector<std::string>::const_iterator key_it = keys.begin();
    for (size_t i = 0; i < channels.size(); i++) {
        join_channel(server, client, channels[i],
                     key_it != keys.end() ? (*key_it).c_str() : NULL);
        if (key_it != keys.end()) ++key_it;
    }
}
