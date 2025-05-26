#include <sstream>
#include <string>
#include <vector>

#include "Channel.hpp"
#include "Server.hpp"
#include "numerics.hpp"
#include "utils.hpp"

void send_msg(Client &author, const std::string &channel, Client &receiver,
              const std::string &content) {
    std::stringstream buffer;

    buffer << ":" << author.get_prefix() << " PRIVMSG " << channel << " :"
           << content;

    receiver.send(buffer.str());
}

void PRIVMSG(std::vector<std::string> components, Client &client,
             Server &server) {
    if (!client.ensure_registerd()) return;

    if (components.size() == 1) {
        client.send_numeric_response("localhost", ERR_NORECIPIENT);

        return;
    }

    if (components.size() == 2) {
        if (is_trailing(components[1])) {
            client.send_numeric_response("localhost", ERR_NORECIPIENT);

        } else {
            client.send_numeric_response("localhost", ERR_NOTEXTTOSEND);
        }

        return;
    }

    std::vector<std::string> recipients = split(components[1], ',');

    if (!is_trailing(components[2])) {
        client.send_numeric_response("localhost", ERR_NOTEXTTOSEND);

        return;
    }

    std::string text = get_trailing_value(components[2]);

    for (std::vector<std::string>::const_iterator it = recipients.begin();
         it != recipients.end(); ++it) {
        if (*it == "") {
            client.send_numeric_response("localhost", ERR_NORECIPIENT);

            continue;
        }

        if ((*it)[0] == '&') {
            Channel *target_channel = server.channelManager().getChannel(*it);
            if (target_channel == NULL) {
                client.send_detailed_numeric_response("localhost", *it,
                                                      ERR_NOSUCHCHANNEL);
                continue;
            }

            if (!target_channel->hasClient(client)) {
                client.send_detailed_numeric_response("localhost", *it,
                                                      ERR_CANNOTSENDTOCHAN);
                continue;
            }

            std::string msg =
                ":" + client.get_prefix() + " PRIVMSG " + *it + " :" + text;
            target_channel->broadcast(client, msg);

            continue;
        }

        Client *recipient = server.clientManager().getClientByNick(*it);
        if (recipient == NULL || !recipient->is_registered()) {
            client.send_detailed_numeric_response("localhost", *it,
                                                  ERR_NOSUCHNICK);

            continue;
        }

        send_msg(client, *it, *recipient, text);
    }
}
