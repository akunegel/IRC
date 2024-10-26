#include "../inc/IRC.hpp"

void handle_topic_command(int client_socket, const std::string &arg, t_data *data) {
    size_t space_pos = arg.find(' ');
    if (space_pos == std::string::npos) {
        send_message(client_socket, "Invalid TOPIC format, usage: TOPIC <channel> <topic>.\n");
        return;
    }
    
    std::string channel_name = arg.substr(0, space_pos);
    std::string new_topic = arg.substr(space_pos + 1);
    if (data->channels.find(channel_name) == data->channels.end()) {
        send_message(client_socket, "Channel " + channel_name + " does not exist.\n");
        return;
    }
    bool topic_restricted = data->topic_restricted[channel_name];
    if (topic_restricted && data->channel_operators[channel_name].find(client_socket) == data->channel_operators[channel_name].end()) {
        send_message(client_socket, "Only operators can change the topic for channel " + channel_name + ".\n");
        return;
    }
    data->channel_topics[channel_name] = new_topic;
    send_message(client_socket, "Topic for channel " + channel_name + " has been set to: " + new_topic + ".\n");
    std::vector<int> &channel_members = data->channels[channel_name];
    for (size_t i = 0; i < channel_members.size(); ++i) {
        send_message(channel_members[i], "Topic for channel " + channel_name + " has been changed to: " + new_topic + ".\n");
    }
}


void handle_kick_command(int client_socket, const std::string &args, t_data *data, t_client &client) {
    size_t space_pos = args.find(' ');
    if (space_pos == std::string::npos) {
        send_message(client_socket, "Usage: KICK <channel> <user>.\n");
        return;
    }
    std::string channel_name = args.substr(0, space_pos);
    std::string target_nickname = args.substr(space_pos + 1);
    if (data->channels.find(channel_name) == data->channels.end()) {
        send_message(client_socket, "Channel " + channel_name + " does not exist.\n");
        return;
    }
    std::vector<int> &channel_members = data->channels[channel_name];
    bool expeller_in_channel = false;
    for (std::vector<int>::iterator it = channel_members.begin(); it != channel_members.end(); ++it) {
        if (*it == client_socket) {
            expeller_in_channel = true;
            break;
        }
    }
    if (!expeller_in_channel) {
        send_message(client_socket, "You are not in the channel " + channel_name + "\n");
        return;
    }
    int target_socket = -1;
    for (std::vector<int>::iterator it = channel_members.begin(); it != channel_members.end(); ++it) {
        int member_socket = *it;
        if (data->clients[member_socket].nickname == target_nickname) {
            target_socket = member_socket;
            break;
        }
    }
    if (target_socket == -1) {
        send_message(client_socket, "User " + target_nickname + " is not in the channel " + channel_name + ".\n");
        return;
    }
    if (data->channel_operators.find(channel_name) == data->channel_operators.end() ||
        data->channel_operators[channel_name].find(client_socket) == data->channel_operators[channel_name].end()) {
        send_message(client_socket, "You are not an operator of channel " + channel_name + "\n");
        send_message(target_socket, "User " + client.nickname + " tried to kick you from " + channel_name + ".\n");
        return;
    }
    for (std::vector<int>::iterator it = channel_members.begin(); it != channel_members.end(); ++it) {
        send_message(*it, client.nickname + " has kicked " + target_nickname + " from " + channel_name + ".\n");
    }
    for (std::vector<int>::iterator it = channel_members.begin(); it != channel_members.end(); ++it) {
        if (*it == target_socket) {
            channel_members.erase(it);
            break;
        }
    }
    send_message(target_socket, "You have been kicked from " + channel_name + ".\n");
    if (channel_members.empty()) {
        data->channels.erase(channel_name);
    }
    data->invited_users[channel_name].erase(target_socket);
}

void handle_invite_command(int client_socket, const std::string &args, t_data *data, t_client &client)
{
    size_t space_pos = args.find(' ');
    if (space_pos == std::string::npos)
    {
        send_message(client_socket, "Usage: INVITE <nickname> <channel>.\n");
        return;
    }
    std::string target_nickname = args.substr(0, space_pos);
    std::string channel_name = args.substr(space_pos + 1);
    if (data->channels.find(channel_name) == data->channels.end())
    {
        send_message(client_socket, "Channel " + channel_name + " does not exist.\n");
        return;
    }
    std::vector<int> &channel_members = data->channels[channel_name];
    bool expeditor_in_channel = false;
    for (size_t i = 0; i < channel_members.size(); ++i)
    {
        if (channel_members[i] == client_socket)
        {
            expeditor_in_channel = true;
            break;
        }
    }
    if (!expeditor_in_channel)
    {
        send_message(client_socket, "You must be in the channel " + channel_name + " to invite others.\n");
        return;
    }
    int target_socket = -1;
    for (std::map<int, t_client>::iterator it = data->clients.begin(); it != data->clients.end(); ++it)
    {
        if (it->second.nickname == target_nickname)
        {
            target_socket = it->first;
            break;
        }
    }
    if (target_socket == -1)
    {
        send_message(client_socket, "User " + target_nickname + " does not exist.\n");
        return;
    }
    for (size_t i = 0; i < channel_members.size(); ++i)
    {
        if (channel_members[i] == target_socket)
        {
            send_message(client_socket, target_nickname + " is already in the channel " + channel_name + ".\n");
            return;
        }
    }
    data->invited_users[channel_name].insert(target_socket);
    send_message(target_socket, "You have been invited by " + client.nickname + " to join the channel " + channel_name + ". Please type JOIN " + channel_name + " to join.\n");
    send_message(client_socket, "Invitation sent to " + target_nickname + " to join " + channel_name + ".\n");
}
