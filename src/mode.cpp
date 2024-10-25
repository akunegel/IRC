#include "../inc/IRC.hpp"

void handle_mode_o(int client_socket, const std::string &args, t_data *data)
{
    size_t space_pos = args.find(' ');
    if (space_pos == std::string::npos)
    {
        send_message(client_socket, "Usage: MODE -o <channel> <nickname>\n");
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

void handle_mode_k(int client_socket, const std::string &channel_name, const std::string &password, t_data *data, t_client &client) {
    if (data->channels.find(channel_name) == data->channels.end()) {
        send_message(client_socket, "Channel " + channel_name + " does not exist.\n");
        return;
    }
    if (data->channel_operators[channel_name].find(client_socket) == data->channel_operators[channel_name].end()) {
        send_message(client_socket, "You must be an operator of the channel to set a password.\n");
        return;
    }
    if (password.empty()) {
        data->passwords.erase(channel_name);
        send_message(client_socket, "Password for channel " + channel_name + " has been removed.\n");
    } else {
        data->passwords[channel_name] = password;
        send_message(client_socket, "Password for channel " + channel_name + " has been set.\n");
    }
}

void handle_mode_command(int client_socket, std::string &arg, t_data *data, t_client &client)
{
    if (arg.empty())
    {
        send_message(client_socket, "Usage: MODE <flag>\n");
        return;
    }

    std::string flag = arg.substr(0, 2);
    arg = arg.substr(3);

    size_t space_pos = arg.find(' ');
    std::string channel_name = arg.substr(0, space_pos);
    std::string additional_arg;

    if (space_pos != std::string::npos)
        additional_arg = arg.substr(space_pos + 1);
    
	(void)client;
    if (flag == "-u")
    {
        //handle_mode_i(client_socket, data, client);
    }
    else if (flag == "-t")
    {
        //handle_mode_t(client_socket, data, client);
    }
    else if (flag == "-k")
    {
        handle_mode_k(client_socket, channel_name, additional_arg, data, client);
	}
    else if (flag == "-o")
    {
        handle_mode_o(client_socket, arg, data);
    }
    else if (flag == "-l")
    {
        //handle_mode_l(client_socket, data, client);
    }
    else
    {
        send_message(client_socket, "Unknown mode flag: " + flag + "\nAvailable flags are: -i, -t, -k, -o, -l\n");
    }
}