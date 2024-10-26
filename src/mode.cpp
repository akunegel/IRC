#include "../inc/IRC.hpp"

void handle_mode_i(int client_socket, std::string &channel_name, t_data *data) {
    if (data->channels.find(channel_name) == data->channels.end()) {
        send_message(client_socket, "Channel " + channel_name + " does not exist.\n");
        return;
    }
    if (data->channel_operators[channel_name].find(client_socket) == data->channel_operators[channel_name].end()) {
        send_message(client_socket, "You must be an operator in the channel " + channel_name + " to set invite restriction.\n");
        return;
    }
    bool is_invite_only = data->invite_only[channel_name];
    data->invite_only[channel_name] = !is_invite_only;
    std::string invite_restriction = is_invite_only ? "disabled" : "enabled";
    send_message(client_socket, "Invite restriction for channel " + channel_name + " " + invite_restriction + ".\n");
}

void handle_mode_t(int client_socket, const std::string &channel_name, t_data *data) {
    if (data->channels.find(channel_name) == data->channels.end()) {
        send_message(client_socket, "Channel " + channel_name + " does not exist.\n");
        return;
    }
    if (data->channel_operators[channel_name].find(client_socket) == data->channel_operators[channel_name].end()) {
        send_message(client_socket, "You must be an operator in the channel " + channel_name + " to set topic restriction.\n");
        return;
    }
    bool is_restricted = data->topic_restricted[channel_name];
    data->topic_restricted[channel_name] = !is_restricted;
    std::string restriction_status = is_restricted ? "disabled" : "enabled";
    send_message(client_socket, "Topic restriction for channel " + channel_name + " " + restriction_status + ".\n");
}


void handle_mode_o(int client_socket, const std::string &args, t_data *data)
{
    size_t space_pos = args.find(' ');
    if (space_pos == std::string::npos)
    {
        send_message(client_socket, "Usage: MODE -o <channel> <nickname>.\n");
        return;
    }
    std::string channel_name = args.substr(0, space_pos);
    std::string target_nickname = args.substr(space_pos + 1);
    if (data->channels.find(channel_name) == data->channels.end())
    {
        send_message(client_socket, "Channel " + channel_name + " does not exist.\n");
        return;
    }
    if (data->channel_operators[channel_name].find(client_socket) == data->channel_operators[channel_name].end())
    {
        send_message(client_socket, "You must be an operator in " + channel_name + " to use MODE -o.\n");
        return;
    }
    int target_socket = -1;
    std::vector<int> &channel_members = data->channels[channel_name];
    for (std::vector<int>::iterator it = channel_members.begin(); it != channel_members.end(); ++it)
    {
        if (data->clients[*it].nickname == target_nickname)
        {
            target_socket = *it;
            break;
        }
    }
    if (target_socket == -1)
    {
        send_message(client_socket, "User " + target_nickname + " is not in channel " + channel_name + ".\n");
        return;
    }
    std::set<int> &operators = data->channel_operators[channel_name];
    if (operators.find(target_socket) != operators.end())
    {
        operators.erase(target_socket);
        send_message(client_socket, target_nickname + " is no longer an operator in " + channel_name + ".\n");
        send_message(target_socket, "You have been removed as an operator in " + channel_name + ".\n");
    }
    else
    {
        operators.insert(target_socket);
        send_message(client_socket, target_nickname + " is now an operator in " + channel_name + ".\n");
        send_message(target_socket, "You have been granted operator privileges in " + channel_name + ".\n");
    }
}

void handle_mode_k(int client_socket, const std::string &channel_name, const std::string &password, t_data *data) {
    if (data->channels.find(channel_name) == data->channels.end()) {
        send_message(client_socket, "Channel " + channel_name + " does not exist.\n");
        return;
    }
    if (data->channel_operators[channel_name].find(client_socket) == data->channel_operators[channel_name].end()) {
        send_message(client_socket, "You must be an operator of the channel to set a password.\n");
        return;
    }
    if (password.empty()) {
        data->channel_passwords.erase(channel_name);
        send_message(client_socket, "Password for channel " + channel_name + " has been removed.\n");
    } else {
        data->channel_passwords[channel_name] = password;
        send_message(client_socket, "Password for channel " + channel_name + " has been set.\n");
    }
}

void handle_mode_l(int client_socket, const std::string &arg, t_data *data) {
    size_t space_pos = arg.find(' ');
    if (space_pos == std::string::npos) {
        send_message(client_socket, "Usage: MODE <channel> -l [limit]\n");
        return;
    }
    
    std::string channel_name = arg.substr(0, space_pos);
    std::string limit_str = arg.substr(space_pos + 1);

    if (data->channels.find(channel_name) == data->channels.end()) {
        send_message(client_socket, "Channel " + channel_name + " does not exist.\n");
        return;
    }

    if (data->channel_operators[channel_name].find(client_socket) == data->channel_operators[channel_name].end()) {
        send_message(client_socket, "You must be an operator in the channel " + channel_name + " to set the user limit.\n");
        return;
    }

    if (limit_str.empty()) {
        data->max_users.erase(channel_name);
        send_message(client_socket, "User limit removed from channel " + channel_name + ".\n");
    } else {
        int limit = atoi(limit_str.c_str());
        if (limit <= 0) {
            send_message(client_socket, "Invalid user limit.\n");
            return;
        }
        data->max_users[channel_name] = limit;
        send_message(client_socket, "User limit for channel " + channel_name + " set to " + limit_str + ".\n");
    }
}



void handle_mode_command(int client_socket, std::string &arg, t_data *data, t_client &client)
{
    if (arg.empty())
    {
        send_message(client_socket, "Usage: MODE <flag>.\n");
        return;
    }

    std::string flag = arg.substr(0, 2);
    arg = arg.substr(3);

    size_t space_pos = arg.find(' ');
    std::string channel_name = arg.substr(0, space_pos);
    std::string additional_arg;
    (void)client;
    if (space_pos != std::string::npos)
        additional_arg = arg.substr(space_pos + 1);
    
    if (flag == "-i")
    {
        handle_mode_i(client_socket, channel_name, data);
    }
    else if (flag == "-t")
    {
        handle_mode_t(client_socket, channel_name, data);
    }
    else if (flag == "-k")
    {
        handle_mode_k(client_socket, channel_name, additional_arg, data);
	}
    else if (flag == "-o")
    {
        handle_mode_o(client_socket, arg, data);
    }
    else if (flag == "-l")
    {
        handle_mode_l(client_socket, arg, data);
    }
    else
    {
        send_message(client_socket, "Unknown mode flag: " + flag + "\nAvailable flags are: -i, -t, -k, -o, -l.\n");
    }
}