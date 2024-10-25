#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <map>
#include <cerrno>
#include <vector>
#include <set>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 512

typedef struct Client {
    int socket;
    bool authenticated;
    std::string nickname;
    std::string username;
    std::string buffer;

    Client() : socket(-1), authenticated(false), username("Anonymous") {}
} t_client;

typedef struct s_data {
    int port;
    std::string password;
    std::map<int, t_client> clients;
    std::map<std::string, std::vector<int> > channels;
    std::map<std::string, std::set<int> > channel_operators;
    std::map<std::string, std::string> channel_topics;
    std::map<std::string, int> max_users;
    std::map<std::string, bool> invite_only;
    std::map<std::string, std::set<int> > invited_users;
    std::map<std::string, std::string> channel_passwords;
    std::map<std::string, bool> topic_restricted;
} t_data;


t_data parse_args(std::string password, std::string port);
void run_server(t_data *data);
void exitWithError(const std::string& message);
void set_socket_non_blocking(int socket);
void send_message(int client_socket, const std::string& message);
void process_command(int client_socket, const std::string& command, t_data* data);
void handle_topic_command(int client_socket, const std::string &arg, t_data *data);
void handle_kick_command(int client_socket, const std::string &args, t_data *data, t_client &client);
void handle_invite_command(int client_socket, const std::string &args, t_data *data, t_client &client);
void handle_mode_command(int client_socket, std::string &arg, t_data *data, t_client &client);