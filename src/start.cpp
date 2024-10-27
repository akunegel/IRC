#include "../inc/IRC.hpp"
#include <sstream>

int user_counter = 0;

int create_server_socket(int port)
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        exitWithError("Error: Cannot create socket.");
    }

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        exitWithError("Error: setsockopt failed.");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        exitWithError("Error: Bind failed");
    }

    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        exitWithError("Error: Listen failed");
    }

    std::cout << "Server listening on port " << port << std::endl;
    return server_socket;
}

void handle_new_connection(int server_socket, struct pollfd *pollfds, int *client_count, t_data *data)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);

    if (client_socket < 0)
    {
        std::cerr << "Error: Accept failed" << std::endl;
        return;
    }

    set_socket_non_blocking(client_socket);

    if (*client_count < MAX_CLIENTS)
    {
        pollfds[*client_count].fd = client_socket;
        pollfds[*client_count].events = POLLIN;

        t_client new_client;
        std::stringstream ss;
        ss << "Anonymous" << user_counter;
        user_counter++;
        new_client.nickname = ss.str();
        new_client.socket = client_socket;
        data->clients[client_socket] = new_client;
        (*client_count)++;
        std::cout << "New client connected!" << std::endl;
        send_message(client_socket, "You must authenticate using the PASS command!\n");
    }
    else
    {
        std::cerr << "Max clients reached, closing connection" << std::endl;
        close(client_socket);
    }
}

void handle_client_message(struct pollfd *pollfd, int *client_count, struct pollfd *pollfds, t_data *data)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    int bytes_read = recv(pollfd->fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_read <= 0)
    {
        std::cerr << "Client disconnected or recv error.\n";
        close(pollfd->fd);
        data->clients.erase(pollfd->fd);
        pollfd->fd = pollfds[*client_count - 1].fd;
        pollfd->events = pollfds[*client_count - 1].events;
        (*client_count)--;
    }
    else
    {
        t_client &client = data->clients[pollfd->fd];
        client.buffer.append(buffer, bytes_read);
        
        size_t newline_pos;
        while ((newline_pos = client.buffer.find('\n')) != std::string::npos)
        {
            std::string command = client.buffer.substr(0, newline_pos);
            client.buffer.erase(0, newline_pos + 1);
            process_command(pollfd->fd, command, data);
            std::cout << "message: " << command << std::endl;
        }
    }
}




void run_server(t_data *data)
{
    int server_socket = create_server_socket(data->port);

    struct pollfd pollfds[MAX_CLIENTS];
    memset(pollfds, 0, sizeof(pollfds));

    pollfds[0].fd = server_socket;
    pollfds[0].events = POLLIN;

    int client_count = 1;

    while (true)
    {
        int poll_result = poll(pollfds, client_count, -1);
        if (poll_result < 0)
        {
            exitWithError("Error: poll failed");
        }

        if (pollfds[0].revents & POLLIN)
        {
            handle_new_connection(server_socket, pollfds, &client_count, data);
        }

        for (int i = 1; i < client_count; ++i)
        {
            if (pollfds[i].revents & POLLIN)
            {
                handle_client_message(&pollfds[i], &client_count, pollfds, data);
            }
        }
    }

    close(server_socket);
}