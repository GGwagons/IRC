#include "CommandManager.hpp"

CommandManager::CommandManager() {
    command_map_["INVITE"]  = INVITE;
    command_map_["JOIN"]    = JOIN;
    command_map_["KICK"]    = KICK;
    command_map_["MODE"]    = MODE;
    command_map_["NICK"]    = NICK;
    command_map_["PART"]    = PART;
    command_map_["PASS"]    = PASS;
    command_map_["PING"]    = PING;
    command_map_["PRIVMSG"] = PRIVMSG;
    command_map_["QUIT"]    = QUIT;
    command_map_["TOPIC"]   = TOPIC;
    command_map_["USER"]    = USER;
}

CommandManager::~CommandManager() {
}

CommandManager::CommandExecutor CommandManager::get(
    const std::string &command) {
    return command_map_[command];
}
