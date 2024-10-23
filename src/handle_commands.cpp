#include "../inc/IRC.hpp"

void handle_pass_command(int client_socket, const std::string &arg, t_data *data, t_client &client) {
    if (arg == data->password) {
        client.authenticated = true;
        send_message(client_socket, "Authentication successful!\n");
    } else {
        send_message(client_socket, "Invalid password!\n");
    }
}

void handle_nick_command(int client_socket, const std::string &arg, t_data *data, t_client &client) {
    bool user_found = false;
    for (std::map<int, t_client>::iterator it = data->clients.begin(); it != data->clients.end(); ++it) {
        if (it->second.nickname == arg) {
            send_message(client_socket, "Nickname not available.\n");
            user_found = true;
            break;
        }
    }
    if (!user_found) {
        client.nickname = arg;
        send_message(client_socket, "Nickname set to " + arg + "\n");
    }
}

void handle_user_command(int client_socket, const std::string &arg, t_client &client) {
    client.username = arg;
    send_message(client_socket, "Username set to " + arg + "\n");
}

void handle_join_command(int client_socket, const std::string &arg, t_data *data, t_client &client) {
    std::vector<int> &channel_members = data->channels[arg];
    if (data->channels.find(arg) == data->channels.end()) {
        if (arg.find(' ') == std::string::npos) {
            data->channels[arg] = std::vector<int>();
            data->channels[arg].push_back(client_socket);
            send_message(client_socket, "You joined the channel " + arg + "\n");
        } else {
            send_message(client_socket, "Invalid channel name, can't have space in name.\n");
        }
    } else {
        for (size_t j = 0; j < channel_members.size(); ++j) {
            send_message(channel_members[j], client.nickname + " has joined the channel " + arg + "\n");
        }
        data->channels[arg].push_back(client_socket);
        send_message(client_socket, "You joined the channel " + arg + "\n");
    }
}

void handle_privmsg_command(int client_socket, const std::string &arg, t_data *data, t_client &client) {
    size_t target_pos = arg.find(' ');
    if (target_pos != std::string::npos) {
        std::string target = arg.substr(0, target_pos);
        std::string message = arg.substr(target_pos + 1);

        if (data->channels.find(target) != data->channels.end()) {
            std::vector<int> &channel_members = data->channels[target];
            bool is_member = false;

            for (size_t i = 0; i < channel_members.size(); ++i) {
                if (channel_members[i] == client_socket) {
                    is_member = true;
                    break;
                }
            }
            if (is_member) {
                for (size_t i = 0; i < channel_members.size(); ++i) {
                    int member_socket = channel_members[i];
                    if (member_socket != client_socket) {
                        send_message(member_socket, client.nickname + ": " + message + "\n");
                    }
                }
            } else {
                send_message(client_socket, "You are not in the channel " + target + "\n");
            }
        }
        else {
            bool user_found = false;
            for (std::map<int, t_client>::iterator it = data->clients.begin(); it != data->clients.end(); ++it) {
                if (it->second.nickname == target) {
                    send_message(it->first, client.nickname + " (private): " + message + "\n");
                    user_found = true;
                }
            }
            if (!user_found) {
                send_message(client_socket, "User " + target + " does not exist.\n");
            }
        }
    } else {
        send_message(client_socket, "Invalid PRIVMSG format\n");
    }
}

void handle_part_command(int client_socket, const std::string &arg, t_data *data, t_client &client) {
    if (data->channels.find(arg) != data->channels.end()) {
        std::vector<int> &channel_members = data->channels[arg];
        bool found = false;

        for (size_t i = 0; i < channel_members.size(); ++i) {
            if (channel_members[i] == client_socket) {
                found = true;
                channel_members.erase(channel_members.begin() + i);
                send_message(client_socket, "You left the channel " + arg + "\n");
                for (size_t j = 0; j < channel_members.size(); ++j) {
                    send_message(channel_members[j], client.nickname + " has left the channel " + arg + "\n");
                }
                if (channel_members.empty()) {
                    data->channels.erase(arg);
                }
                break;
            }
        }

        if (!found) {
            send_message(client_socket, "You are not in the channel " + arg + "\n");
        }
    } else {
        send_message(client_socket, "Channel " + arg + " does not exist\n");
    }
}

void process_command(int client_socket, const std::string &command, t_data *data) {
    t_client &client = data->clients[client_socket];

    std::string prefix, arg;
    size_t space_pos = command.find(' ');
    if (space_pos != std::string::npos) {
        prefix = command.substr(0, space_pos);
        arg = command.substr(space_pos + 1);
        arg.erase(arg.find_last_not_of(" \r\n") + 1);
    } else {
        prefix = command;
    }

    if (prefix == "PASS") {
        handle_pass_command(client_socket, arg, data, client);
    } else if (client.authenticated) {
        if (prefix == "NICK") {
            handle_nick_command(client_socket, arg, data, client);
        } else if (prefix == "USER") {
            handle_user_command(client_socket, arg, client);
        } else if (prefix == "JOIN") {
            handle_join_command(client_socket, arg, data, client);
        } else if (prefix == "PRIVMSG") {
            handle_privmsg_command(client_socket, arg, data, client);
        } else if (prefix == "PART") {
            handle_part_command(client_socket, arg, data, client);
        } else {
            send_message(client_socket, "Unknown command\n");
        }
    } else {
        send_message(client_socket, "You must authenticate first using the PASS command!\n");
    }
}
