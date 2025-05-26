#include "ChannelManager.hpp"

#include <stdexcept>

#include "Channel.hpp"
#include "Client.hpp"
#include "numerics.hpp"

ChannelManager::ChannelManager() {
}

ChannelManager::ChannelManager(const ChannelManager &copy) {
    *this = copy;
}

ChannelManager &ChannelManager::operator=(const ChannelManager &copy) {
    if (this != &copy) {
        _channels = copy._channels;
    }
    return (*this);
}

ChannelManager::~ChannelManager() {
    for (ChannelMap::iterator it = _channels.begin(); it != _channels.end();
         ++it) {
        delete it->second;
    }

    _channels.clear();
}

void ChannelManager::createChannel(Client            *client,
                                   const std::string &channelName,
                                   const char        *key) {
    if (_channels.find(channelName) == _channels.end()) {
        Channel *channel = new Channel(channelName);
        try {
            _channels[channelName] = channel;
        } catch (...) {
            delete channel;

            throw;
        }
        _channels[channelName]->makeOperator(*client);
    }

    Channel *channel = getChannel(channelName);

    if (channel->hasClient(*client)) {
        return;
    }

    if (channel->keyMode()) {
        if (key == NULL || !channel->isValidKey(key)) {
            client->send_detailed_numeric_response("localhost", channelName,
                                                   ERR_BADCHANNELKEY);
            return;
        }
    }

    if (channel->inviteOnlyMode() && !channel->isInvited(*client)) {
        client->send_detailed_numeric_response("localhost", channelName,
                                               ERR_INVITEONLYCHAN);
        return;
    }

    if (channel->isFull()) {
        client->send_detailed_numeric_response("localhost", channelName,
                                               ERR_CHANNELISFULL);
        return;
    }

    channel->addClient(client);

    if (channel->hasTopic()) {
        std::string reply = ":localhost 332 " + client->getNickName() + " " +
                            channelName + " :" + channel->getTopic();
        client->send(reply);

        reply = ":localhost 333 " + client->getNickName() + " " +
                channel->getTopicWhotime();
        client->send(reply);
    } else {
        client->send_detailed_numeric_response("localhost", channelName,
                                               RPL_NOTOPIC);
    }

    channel->sendNamesList(*client);
}

bool ChannelManager::hasChannel(const std::string &channelName) {
    return _channels.find(channelName) != _channels.end();
}

Channel *ChannelManager::getChannel(const std::string &channelName) {
    if (!hasChannel(channelName)) return NULL;

    return _channels[channelName];
}

void ChannelManager::removeChannel(const std::string &channelName) {
    Channel *channel = getChannel(channelName);
    if (channel == NULL) {
        return;
    }

    _channels.erase(channelName);

    delete channel;
}

std::set<Channel *> ChannelManager::clientChannels(const Client &client) {
    std::set<Channel *> client_channels;

    for (ChannelMap::iterator it = _channels.begin(); it != _channels.end();
         ++it) {
        if (!it->second->hasClient(client)) continue;

        client_channels.insert(it->second);
    }

    return client_channels;
}

void ChannelManager::removeUser(Channel &channel, const Client &client) {
    if (!channel.hasClient(client)) {
        return;
    }

    channel.removeClient(client);

    if (channel.empty()) {
        removeChannel(channel.getChannelName());
    }
}

void ChannelManager::removeUserFromAllChannels(const Client &client) {
    std::set<Channel *> client_channels = clientChannels(client);

    for (std::set<Channel *>::iterator it = client_channels.begin();
         it != client_channels.end(); ++it) {
        removeUser(**it, client);
    }
}

bool ChannelManager::kickUser(Channel &channel, const Client &author,
                              const Client      &to_kick,
                              const std::string &reason) {
    if (!channel.hasClient(to_kick)) {
        std::string msg =
            to_kick.getNickName() + " " + channel.getChannelName();
        author.send_detailed_numeric_response("localhost", msg,
                                              ERR_USERNOTINCHANNEL);

        return false;
    }

    std::string msg = ":" + author.get_prefix() + " KICK " +
                      channel.getChannelName() + " " + to_kick.getNickName() +
                      " :" + reason;
    channel.broadcastAll(msg);

    channel.removeClient(to_kick);

    if (channel.empty()) {
        removeChannel(channel.getChannelName());
    }

    return true;
}

//--------------------------------------------------SENDING
// MESSAGES--------------------------------------------------

void ChannelManager::broadcastSharedUnique(Client     &client,
                                           std::string message) {
    std::set<Channel *>      client_channels = clientChannels(client);
    std::set<const Client *> already_received;

    for (std::set<Channel *>::iterator it = client_channels.begin();
         it != client_channels.end(); ++it) {
        for (std::set<const Client *>::const_iterator client_it =
                 (*it)->clients().begin();
             client_it != (*it)->clients().end(); ++client_it) {
            if (*client_it == &client) continue;

            if (already_received.find(*client_it) != already_received.end())
                continue;

            (*client_it)->send(message);
            already_received.insert(*client_it);
        }
    }
}

void ChannelManager::broadcastSharedAllUnique(Client     &client,
                                              std::string message) {
    std::set<Channel *>      client_channels = clientChannels(client);
    std::set<const Client *> already_received;

    for (std::set<Channel *>::iterator it = client_channels.begin();
         it != client_channels.end(); ++it) {
        for (std::set<const Client *>::const_iterator client_it =
                 (*it)->clients().begin();
             client_it != (*it)->clients().end(); ++client_it) {
            if (already_received.find(*client_it) != already_received.end())
                continue;

            (*client_it)->send(message);
            already_received.insert(*client_it);
        }
    }
}
