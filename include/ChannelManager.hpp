#pragma once

#include <map>
#include <set>
#include <string>

class Channel;
class Client;

class ChannelManager {
   private:
    typedef std::map<std::string, Channel *> ChannelMap;
    ChannelMap                               _channels;

   public:
    ChannelManager();
    ChannelManager(const ChannelManager &copy);
    ChannelManager &operator=(const ChannelManager &copy);
    ~ChannelManager();

    void     createChannel(Client *client, const std::string &channelName,
                           const char *key);
    bool     hasChannel(const std::string &channelName);
    Channel *getChannel(const std::string &channelName);
    void     removeChannel(const std::string &channelName);

    std::set<Channel *> clientChannels(const Client &client);

    void removeUser(Channel &channel, const Client &client);
    void removeUserFromAllChannels(const Client &client);
    bool kickUser(Channel &channel, const Client &author, const Client &to_kick,
                  const std::string &reason);

    void broadcastSharedUnique(Client &client, std::string message);
    void broadcastSharedAllUnique(Client &client, std::string message);
};
