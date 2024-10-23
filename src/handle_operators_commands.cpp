#include "../inc/IRC.hpp"

void handle_topic_command(int client_socket, const std::string &arg, t_data *data)
{
    size_t space_pos = arg.find(' ');
    if (space_pos == std::string::npos)
    {
        send_message(client_socket, "Invalid TOPIC format, usage: TOPIC <channel> <topic>\n");
        return;
    }
    std::string channel_name = arg.substr(0, space_pos);
    std::string new_topic = arg.substr(space_pos + 1);
    if (data->channels.find(channel_name) == data->channels.end())
    {
        send_message(client_socket, "Channel " + channel_name + " does not exist\n");
        return;
    }
    if (data->channel_operators[channel_name].find(client_socket) == data->channel_operators[channel_name].end())
    {
        send_message(client_socket, "You are not an operator of the channel " + channel_name + "\n");
        return;
    }
    data->channel_topics[channel_name] = new_topic;
    send_message(client_socket, "Topic for channel " + channel_name + " has been set to: " + new_topic + "\n");
    std::vector<int> &channel_members = data->channels[channel_name];
    for (size_t i = 0; i < channel_members.size(); ++i)
    {
        send_message(channel_members[i], "Topic for channel " + channel_name + " has been changed to: " + new_topic + "\n");
    }
}