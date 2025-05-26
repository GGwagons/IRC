#include "Channel.hpp"

#include <ctime>
#include <limits>
#include <sstream>

#include "numerics.hpp"

Channel::Channel(std::string name)
    : _clientLimitMode(false),
      _inviteOnlyMode(false),
      _keyMode(false),
      _protectedTopicMode(false),
      _clientLimit(std::numeric_limits<int>::max()),
      _channelName(name),
      _key(""),
      _topic(""),
      _topicAuthorNick(""),
      _createdTimestamp(std::time(0)),
      _topicTimestamp(0) {
}

Channel::~Channel() {
}

const std::string &Channel::getChannelName() const {
    return _channelName;
}

time_t Channel::createdAt() const {
    return _createdTimestamp;
}

void Channel::addClient(Client *client) {
    _users.insert(client);
    _invitedClients.erase(client->getNickName());

    std::string message = ":" + client->get_prefix() + " JOIN " + _channelName;
    for (std::set<const Client *>::const_iterator it = _users.begin();
         it != _users.end(); ++it) {
        (*it)->send(message);
    }
}

void Channel::removeClient(const Client &client) {
    _users.erase(&client);

    _operators.erase(&client);
}

bool Channel::hasClient(const Client &client) const {
    return _users.find(&client) != _users.end();
}

const std::set<const Client *> &Channel::clients() const {
    return _users;
}

bool Channel::empty() const {
    return _users.empty();
}

void Channel::makeOperator(const Client &client) {
    _operators.insert(&client);
}

void Channel::takeOperator(const Client &client) {
    _operators.erase(&client);
}

bool Channel::isOperator(const Client &client) const {
    return _operators.find(&client) != _operators.end();
}

void Channel::setInviteOnlyMode(bool inviteOnly) {
    if (inviteOnly == true && _inviteOnlyMode == false) {
        _invitedClients.clear();
    }

    _inviteOnlyMode = inviteOnly;
}

bool Channel::inviteOnlyMode() const {
    return _inviteOnlyMode;
}

void Channel::inviteUser(const std::string &nick) {
    _invitedClients.insert(nick);
}

bool Channel::isInvited(const Client &client) const {
    return _invitedClients.find(client.getNickName()) != _invitedClients.end();
}

void Channel::setProtectedTopicMode(bool protectedTopic) {
    _protectedTopicMode = protectedTopic;
}
bool Channel::protectedMode() const {
    return _protectedTopicMode;
}

void Channel::setTopic(const Client &author, const std::string &topic) {
    _topic           = topic;
    _topicAuthorNick = author.getNickName();
    _topicTimestamp  = std::time(0);
}

bool Channel::hasTopic() const {
    return _topic != "";
}

const std::string &Channel::getTopic() const {
    return _topic;
}

const std::string Channel::getTopicWhotime() const {
    std::ostringstream whotime;
    whotime << _channelName << " " << _topicAuthorNick << " "
            << _topicTimestamp;

    return whotime.str();
}

void Channel::enableKeyMode(const std::string &key) {
    _keyMode = true;
    _key     = key;
}

void Channel::disableKeyMode() {
    _keyMode = false;
    _key     = "";
}

bool Channel::keyMode() const {
    return _keyMode;
}

const std::string &Channel::key() const {
    return _key;
}

bool Channel::isValidKey(const std::string &key) const {
    if (_keyMode == false) return true;

    return _key == key;
}

void Channel::enableClientLimitMode(const int clientLimit) {
    _clientLimitMode = true;
    _clientLimit     = clientLimit;
}

void Channel::disableClientLimitMode() {
    _clientLimitMode = false;
    _clientLimit     = std::numeric_limits<int>::max();
}

bool Channel::clientLimitMode() const {
    return _clientLimitMode;
}

int Channel::clientLimit() const {
    return _clientLimit;
}

bool Channel::isFull() const {
    return _users.size() >= (size_t)_clientLimit;
}

void Channel::broadcast(const Client      &author,
                        const std::string &message) const {
    for (std::set<const Client *>::const_iterator it = _users.begin();
         it != _users.end(); ++it) {
        if (*it == &author) continue;

        (*it)->send(message);
    }
}

void Channel::broadcastAll(const std::string &message) const {
    for (std::set<const Client *>::const_iterator it = _users.begin();
         it != _users.end(); ++it) {
        (*it)->send(message);
    }
}

void Channel::sendNamesList(const Client &target) const {
    for (std::set<const Client *>::const_iterator it = _users.begin();
         it != _users.end(); ++it) {
        std::stringstream reply;
        std::string       name = (*it)->getNickName();
        if (isOperator(**it)) name = "@" + name;

        reply << ":localhost " << RPL_NAMREPLY << " " << target.getNickName()
              << " = " << _channelName << " :" << name;

        target.send(reply.str());
    }

    target.send_detailed_numeric_response("localhost", _channelName,
                                          RPL_ENDOFNAMES);
}

bool Channel::is_valid_channel_name(const std::string &channel_name) {
    if (channel_name.empty()) return false;
    if (channel_name.size() > 50) return false;
    if (channel_name.size() < 2) return false;

    std::string::const_iterator it = channel_name.begin();

    // We only support local channels
    if (*it != '&') return false;
    ++it;

    for (; it != channel_name.end(); ++it) {
        if (*it == ' ') return false;
        // Bell Character
        if (*it == '\a') return false;
        if (*it == ',') return false;
    }

    return true;
}
