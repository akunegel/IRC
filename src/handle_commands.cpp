#include "../inc/IRC.hpp"

void process_command(int client_socket, const std::string &command, t_data *data)
{
    t_client &client = data->clients[client_socket];

    std::string prefix, arg;
    size_t space_pos = command.find(' ');
    if (space_pos != std::string::npos)
    {
        prefix = command.substr(0, space_pos);
        arg = command.substr(space_pos + 1);
        arg.erase(arg.find_last_not_of(" \r\n") + 1);
    }
    else
    {
        prefix = command;
    }

    if (prefix == "PASS")
    {
        if (arg == data->password)
        {
            client.authenticated = true;
            send_message(client_socket, "Authentication successful!\n");
        }
        else
        {
            send_message(client_socket, "Invalid password!\n");
            close(client_socket);
            data->clients.erase(client_socket);
        }
    }
    else if (client.authenticated)
    {
        if (prefix == "NICK")
        {
            client.nickname = arg;
            send_message(client_socket, "Nickname set to " + arg + "\n");
        }
        else if (prefix == "USER")
        {
            client.username = arg;
            send_message(client_socket, "Username set to " + arg + "\n");
        }
        else if (prefix == "JOIN")
        {
            if (data->channels.find(arg) == data->channels.end())
            {
                data->channels[arg] = std::vector<int>();
            }
            data->channels[arg].push_back(client_socket);
            send_message(client_socket, "You joined the channel " + arg + "\n");
        }
        else if (prefix == "PRIVMSG")
        {
            size_t channel_pos = arg.find(' ');
            if (channel_pos != std::string::npos)
            {
                std::string channel_name = arg.substr(0, channel_pos);
                std::string message = arg.substr(channel_pos + 1);

                if (data->channels.find(channel_name) != data->channels.end())
                {
                    for (size_t i = 0; i < data->channels[channel_name].size(); i++)
                    {
                        int member_socket = data->channels[channel_name][i];
                        if (member_socket != client_socket)
                        {
                            send_message(member_socket, client.nickname + ": " + message + "\n");
                        }
                    }
                }
                else
                {
                    send_message(client_socket, "Channel " + channel_name + " does not exist.\n");
                }
            }
            else
            {
                send_message(client_socket, "Invalid PRIVMSG format\n");
            }
        }
        else
        {
            send_message(client_socket, "Unknown command\n");
        }
    }
    else
    {
        send_message(client_socket, "You must authenticate first using the PASS command!\n");
    }
}