#pragma once

#include <ctime>
#include <set>

#include "ChannelManager.hpp"
#include "Server.hpp"

class Channel {
   private:
    bool                     _clientLimitMode;
    bool                     _inviteOnlyMode;
    bool                     _keyMode;
    bool                     _protectedTopicMode;
    int                      _clientLimit;
    std::set<const Client *> _operators;
    std::set<const Client *> _users;
    std::set<std::string>    _invitedClients;
    std::string              _channelName;
    std::string              _key;
    std::string              _topic;
    std::string              _topicAuthorNick;
    time_t                   _createdTimestamp;
    time_t                   _topicTimestamp;

    Channel(const Channel &copy);
    Channel &operator=(const Channel &copy);

   public:
    static bool is_valid_channel_name(const std::string &channel_name);

    Channel(std::string name);
    ~Channel();
    const std::string &getChannelName() const;
    time_t             createdAt() const;

    void                            addClient(Client *client);
    void                            removeClient(const Client &client);
    bool                            hasClient(const Client &client) const;
    bool                            empty() const;
    const std::set<const Client *> &clients() const;

    void makeOperator(const Client &client);
    void takeOperator(const Client &client);
    bool isOperator(const Client &client) const;

    void setInviteOnlyMode(bool inviteOnly);
    bool inviteOnlyMode() const;
    void inviteUser(const std::string &nick);
    bool isInvited(const Client &client) const;

    void               setProtectedTopicMode(bool protectedTopic);
    bool               protectedMode() const;
    void               setTopic(const Client &author, const std::string &topic);
    bool               hasTopic() const;
    const std::string &getTopic() const;
    const std::string  getTopicWhotime() const;

    void               enableKeyMode(const std::string &key);
    void               disableKeyMode();
    bool               keyMode() const;
    const std::string &key() const;
    bool               isValidKey(const std::string &key) const;

    void enableClientLimitMode(const int clientLimit);
    void disableClientLimitMode();
    bool clientLimitMode() const;
    int  clientLimit() const;
    bool isFull() const;

    void broadcast(const Client &author, const std::string &message) const;
    void broadcastAll(const std::string &message) const;
    void sendNamesList(const Client &target) const;
};
