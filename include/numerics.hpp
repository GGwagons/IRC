#pragma once

#ifndef NUMERICS_HPP_
#    define NUMERICS_HPP_

#    include <string>

// Success codes
#    define RPL_WELCOME    001  // Sent when a user successfully connects.
#    define RPL_YOURHOST   002  // Server information.
#    define RPL_CREATED    003  // Shows when the server was created.
#    define RPL_MYINFO     004  // Server details (supported modes).
#    define RPL_TOPIC      332  // Shows the current topic of a channel.
#    define RPL_NOTOPIC    331  // If no topic is set in a channel.
#    define RPL_NAMREPLY   353  // Lists all users in a channel.
#    define RPL_ENDOFNAMES 366  // Marks the end of the user list in a channel.
#    define RPL_INVITING   341  // Confirmation that a user has been invited.
#    define RPL_MOTDSTART \
        375  // Start of the server's Message of the Day (MOTD).
#    define RPL_MOTD      372  // Displays a line of the MOTD.
#    define RPL_ENDOFMOTD 376  // End of the MOTD message.

// Error codes
#    define ERR_NOSUCHNICK \
        401  // When a client tries to message a non-existent user.
#    define ERR_NOSUCHCHANNEL \
        403  // When a client tries to join a non-existent channel.
#    define ERR_CANNOTSENDTOCHAN \
        404  // If a client tries to send a message to a restricted/moderated
             // channel.
#    define ERR_NORECIPIENT    411  // If no recipient was given
#    define ERR_NOTEXTTOSEND   412  // No text to send given
#    define ERR_INPUTTOOLONG   417  // Message too long
#    define ERR_UNKNOWNCOMMAND 421  // If a client sends an unknown command.
#    define ERR_NONICKNAMEGIVEN \
        431  // When a client does not provide a nickname.
#    define ERR_ERRONEUSNICKNAME \
        432                        // When a nickname contains invalid
                                   // characters.
#    define ERR_NICKNAMEINUSE 433  // When a nickname is already taken.
#    define ERR_USERNOTINCHANNEL \
        441  // If a client tries to message a user who is not in the channel.
#    define ERR_NOTONCHANNEL \
        442  // If a client tries to send a message to a channel they haven’t
             // joined.
#    define ERR_USERONCHANNEL 443  // Client already on channel.
#    define ERR_NOTREGISTERED \
        451  // If a user tries to perform an action that requires
             // authentication before authenticating.
#    define ERR_NEEDMOREPARAMS \
        461  // When a command is missing required
             // parameters.
#    define ERR_ALREADYREGISTERED \
        462  // If a client tries to register again after connecting.
#    define ERR_PASSWDMISMATCH 464  // If a client enters the wrong password.
#    define ERR_CHANNELISFULL  471  // If the channel has reached its user limit.
#    define ERR_UNKNOWNMODE    472  // Mode char unknown.
#    define ERR_INVITEONLYCHAN \
        473  // If a user tries to join an invite-only channel without an
             // invite.
#    define ERR_BANNEDFROMCHAN 474  // If a user is banned from a channel.
#    define ERR_BADCHANNELKEY \
        475  // If a user enters the wrong password for a channel.
#    define ERR_BADCHANMASK 476  // If the channel name is not valid
#    define ERR_CHANOPRIVSNEEDED \
        482  // If a user tries to perform a channel operator action without
             // being an operator.

#    define ERR_UMODEUNKNOWNFLAG 501  // Mode flag is not known
#    define ERR_USERSDONTMATCH \
        502  // Mode command was send with different user as target
#    define ERR_INVALIDKEY 525  // Invalid channel key provided to MODE

struct Code {
    int         code;
    std::string message;
};

static Code Messages[] = {
    {RPL_WELCOME, "Welcome to the IRC Network"},
    {RPL_YOURHOST, "Your host is irc.server.net, running version 1.0"},
    {RPL_CREATED, "This server was created <date>"},
    {RPL_MYINFO, "irc.server.net v1.0 <user_modes> <channel_modes>"},
    {RPL_TOPIC, "<channel> :<topic>"},
    {RPL_NOTOPIC, "No topic is set"},
    {RPL_NAMREPLY, "<channel> :<user_list>"},
    {RPL_ENDOFNAMES, "End of /NAMES list."},
    {RPL_INVITING, "<channel> <target_nick>"},
    {RPL_MOTDSTART, ":- Message of the Day -"},
    {RPL_MOTD, ":- <MOTD line>"},
    {RPL_ENDOFMOTD, " :End of MOTD."},
    {ERR_NOSUCHNICK, "No such nick/channel"},
    {ERR_NOSUCHCHANNEL, "No such channel"},
    {ERR_CANNOTSENDTOCHAN, "Cannot send to channel"},
    // We do not support `SUMMON` command, therefore hardcoded PRIVMSG
    {ERR_NORECIPIENT, "No recipient given (PRIVMSG)"},
    {ERR_NOTEXTTOSEND, "No text to send"},
    {ERR_INPUTTOOLONG, "Input line was too long"},
    {ERR_UNKNOWNCOMMAND, "Unknown command"},
    {ERR_NONICKNAMEGIVEN, "No nickname given"},
    {ERR_ERRONEUSNICKNAME, "Erroneous nickname"},
    {ERR_NICKNAMEINUSE, "Nickname is already in use"},
    {ERR_USERNOTINCHANNEL, "They aren't on that channel"},
    {ERR_NOTONCHANNEL, "You're not on that channel"},
    {ERR_USERONCHANNEL, "is already on channel"},
    {ERR_NOTREGISTERED, "You have not registered"},
    {ERR_NEEDMOREPARAMS, "Not enough parameters"},
    {ERR_ALREADYREGISTERED, "You may not reregister"},
    {ERR_PASSWDMISMATCH, "Password incorrect"},
    {ERR_CHANNELISFULL, "Cannot join channel (+l)"},
    {ERR_UNKNOWNMODE, "is unknown mode char to me"},
    {ERR_INVITEONLYCHAN, "Cannot join channel (+i)"},
    {ERR_BANNEDFROMCHAN, "<channel> :Cannot join channel (+b)"},
    {ERR_BADCHANNELKEY, "Cannot join channel (+k)"},
    {ERR_BADCHANMASK, "Bad Channel Mask"},
    {ERR_CHANOPRIVSNEEDED, "You're not channel operator"},
    {ERR_UMODEUNKNOWNFLAG, "Unknown MODE flag"},
    {ERR_USERSDONTMATCH, "Cant change mode for other users"},
    {ERR_INVALIDKEY, "Key is not well-formed"},
};

const int COUNT = sizeof(Messages) / sizeof(Messages[0]);

template <typename T>
std::string getMessage(T Code) {
    for (int i = 0; i < COUNT; i++) {
        if (Messages[i].code == Code) {
            return Messages[i].message;
        }
    }
    return "";
}

#endif
