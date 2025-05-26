#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Channel.hpp"
#include "Server.hpp"
#include "numerics.hpp"
#include "utils.hpp"

static void execute_client_mode(std::vector<std::string> &components,
                                Client &client, Server &server,
                                const std::string &target) {
    if (server.clientManager().getClientByNick(target) == NULL) {
        client.send_detailed_numeric_response("localhost", target,
                                              ERR_NOSUCHNICK);
        return;
    }

    if (client.getNickName() != target) {
        client.send_numeric_response("localhost", ERR_USERSDONTMATCH);
        return;
    }

    if (components.size() > 2) {
        client.send_numeric_response("localhost", ERR_UMODEUNKNOWNFLAG);
    }

    client.send(":localhost 221 " + client.getNickName());
}

static std::string create_modestring(const Channel &channel) {
    std::stringstream        modestring;
    std::vector<std::string> mode_arguments;

    modestring << "+";

    if (channel.inviteOnlyMode()) modestring << "i";

    if (channel.protectedMode()) modestring << "t";

    if (channel.clientLimitMode()) {
        modestring << "l";
        std::stringstream argument;
        argument << channel.clientLimit();
        mode_arguments.push_back(argument.str());
    }

    if (channel.keyMode()) {
        modestring << "k";
    }

    for (std::vector<std::string>::const_iterator it = mode_arguments.begin();
         it != mode_arguments.end(); ++it) {
        modestring << " ";
        modestring << *it;
    }

    return modestring.str();
}

static bool is_valid_key(const std::string &key) {
    if (key.empty()) return false;
    if (key.size() > 50) return false;

    for (std::string::const_iterator it = key.begin(); it != key.end(); ++it) {
        if (!std::isprint(*it)) return false;

        switch (*it) {
            case ':':
            case ',':
            case ' ':
                return false;
        }
    }

    return true;
}

static int parse_int(const std::string &raw) {
    for (size_t i = 0; i < raw.length(); i++) {
        if (isdigit(raw[i])) continue;

        return -1;
    }

    std::stringstream stream(raw);
    int               num;

    stream >> num;

    if (stream.fail()) return -1;
    if (!stream.eof()) return -1;

    return num;
}

static void execute_channel_mode(std::vector<std::string> &components,
                                 Client &client, Server &server,
                                 const std::string &target) {
    Channel *channel = server.channelManager().getChannel(target);
    if (channel == NULL) {
        client.send_detailed_numeric_response("localhost", target,
                                              ERR_NOSUCHCHANNEL);
        return;
    }

    if (!channel->hasClient(client)) {
        client.send_detailed_numeric_response(
            "localhost", channel->getChannelName(), ERR_NOTONCHANNEL);
        return;
    }

    if (components.size() > 2) {
        if (!channel->isOperator(client)) {
            client.send_detailed_numeric_response(
                "localhost", channel->getChannelName(), ERR_CHANOPRIVSNEEDED);
            return;
        }

        const std::string &modestring = get_trailing_value(components[2]);

        // TBH should not be possible
        if (modestring.empty()) return;

        std::string::const_iterator it = modestring.begin();
        while (it != modestring.end() && *it != '+' && *it != '-') {
            client.send_detailed_numeric_response(
                "localhost", std::string(1, *it), ERR_UNKNOWNMODE);

            ++it;
        }

        if (it == modestring.end()) return;

        std::vector<std::string>::const_iterator args_it = components.begin();
        args_it += 3;

        std::stringstream        reply_modestring;
        std::vector<std::string> reply_mode_arguments;

        // `*it` can only be '+' or '-'
        bool add_mode        = *it == '+';
        bool needs_mode_char = true;
        ++it;
        for (; it != modestring.end(); ++it) {
            switch (*it) {
                case '+':
                    if (needs_mode_char == true)
                        client.send_detailed_numeric_response(
                            "localhost", std::string(1, *it), ERR_UNKNOWNMODE);
                    add_mode        = true;
                    needs_mode_char = true;
                    break;
                case '-':
                    if (needs_mode_char == true)
                        client.send_detailed_numeric_response(
                            "localhost", std::string(1, *it), ERR_UNKNOWNMODE);
                    add_mode        = false;
                    needs_mode_char = true;
                    break;
                case 'i':
                    needs_mode_char = false;
                    channel->setInviteOnlyMode(add_mode);
                    reply_modestring << (add_mode ? '+' : '-') << 'i';

                    break;
                case 't':
                    needs_mode_char = false;
                    channel->setProtectedTopicMode(add_mode);
                    reply_modestring << (add_mode ? '+' : '-') << 't';

                    break;
                case 'k':
                    needs_mode_char = false;
                    if (!add_mode) {
                        reply_modestring << "-k";
                        channel->disableKeyMode();
                        break;
                    }

                    if (args_it == components.end()) break;

                    if (!is_valid_key(get_trailing_value(*args_it))) {
                        client.send_detailed_numeric_response(
                            "localhost", channel->getChannelName(),
                            ERR_INVALIDKEY);

                        break;
                    }

                    reply_modestring << "+k";

                    channel->enableKeyMode(get_trailing_value(*args_it));
                    ++args_it;

                    break;
                case 'o': {
                    needs_mode_char = false;
                    if (args_it == components.end()) break;

                    Client *target = server.clientManager().getClientByNick(
                        get_trailing_value(*args_it));
                    if (target == NULL) break;

                    if (!channel->hasClient(*target)) break;

                    reply_modestring << (add_mode ? '+' : '-') << 'o';
                    reply_mode_arguments.push_back(
                        get_trailing_value(*args_it));

                    if (!add_mode) {
                        channel->takeOperator(*target);
                        break;
                    }

                    channel->makeOperator(*target);
                    ++args_it;

                    break;
                }
                case 'l': {
                    needs_mode_char = false;
                    if (!add_mode) {
                        reply_modestring << "-l";
                        channel->disableClientLimitMode();
                        break;
                    }

                    if (args_it == components.end()) break;

                    int limit = parse_int(get_trailing_value(*args_it));
                    if (limit == -1) break;

                    reply_modestring << "+l";

                    channel->enableClientLimitMode(limit);
                    std::stringstream res;
                    res << limit;
                    reply_mode_arguments.push_back(res.str());
                    ++args_it;

                    break;
                }
                default:
                    needs_mode_char = false;
                    client.send_detailed_numeric_response(
                        "localhost", std::string(1, *it), ERR_UNKNOWNMODE);
                    continue;
            }
        }

        std::ostringstream reply;
        reply << ":" << client.get_prefix() << " MODE "
              << channel->getChannelName() << " " << reply_modestring.str();

        for (std::vector<std::string>::const_iterator args_it =
                 reply_mode_arguments.begin();
             args_it != reply_mode_arguments.end(); ++args_it) {
            reply << " " << *args_it;
        }

        channel->broadcastAll(reply.str());
        return;
    }

    std::ostringstream reply;
    reply << ":localhost 324 " << client.getNickName() << " "
          << channel->getChannelName() << " " << create_modestring(*channel);
    client.send(reply.str());

    reply.clear();
    reply << ":localhost 329 " << client.getNickName() << " "
          << channel->getChannelName() << " " << channel->createdAt();
    client.send(reply.str());
}

void MODE(std::vector<std::string> components, Client &client, Server &server) {
    if (!client.ensure_registerd()) return;

    if (components.size() == 1) {
        client.send_detailed_numeric_response("localhost", "MODE",
                                              ERR_NEEDMOREPARAMS);
        return;
    }

    const std::string &target = get_trailing_value(components[1]);
    if (target.empty()) {
        client.send_numeric_response("localhost", ERR_USERSDONTMATCH);
        return;
    }

    const bool is_channel = target[0] == '&';
    if (is_channel) {
        execute_channel_mode(components, client, server, target);
    } else {
        execute_client_mode(components, client, server, target);
    }
}
