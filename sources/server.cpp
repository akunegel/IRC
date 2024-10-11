#include "../includes/IRC.hpp"

int createServerSocket(int port)
{
	int servSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (servSocket < 0)
	{
		std::cerr << "Error: Socket creation failed." << std::endl;
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in servAddr;
	std::memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(servSocket, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
	{
		std::cerr << "Error: Bind error." << std::endl;
		close(servSocket);
		exit(0);
	}

	if (listen(servSocket, 50) < 0)
	{
		std::cerr << "Error: Listen error." << std::endl;
		close(servSocket);
		exit(0);
	}

	std::cout << "Server listening on port " << port << std::endl;
	return servSocket;
}

int acceptNewConnection(int servSocket)
{
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);

	int clientSocket = accept(servSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (clientSocket < 0)
	{
		std::cerr << "Error: Accept error." << std::endl;
		return -1;
	}

	char *clientIP = inet_ntoa(clientAddr.sin_addr);
	std::cout << "Connection accepted from: " << clientIP
			  << " on port " << ntohs(clientAddr.sin_port) << std::endl;

	return clientSocket;
}

void handleClientCommunication(int clientSocket)
{
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);

	int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
	if (bytesReceived > 0)
	{
		std::cout << "Message received: " << buffer << std::endl;
		send(clientSocket, "Message received.", 17, 0);
	}

	close(clientSocket);
}

void runServerLoop(int servSocket)
{
	std::vector<struct pollfd> pollFds;

	struct pollfd serverFd;
	serverFd.fd = servSocket;
	serverFd.events = POLLIN;
	pollFds.push_back(serverFd);

	while (true) {
		int pollCount = poll(pollFds.data(), pollFds.size(), -1);
		if (pollCount < 0)
		{
			std::cerr << "Error: Poll error." << std::endl;
			exit(0);
		}

		for (std::size_t i = 0; i < pollFds.size(); i++) {
			if (pollFds[i].fd == servSocket && pollFds[i].revents & POLLIN) {
				int clientSocket = acceptNewConnection(servSocket);
				if (clientSocket >= 0) {
					struct pollfd clientFd;
					clientFd.fd = clientSocket;
					clientFd.events = POLLIN;
					pollFds.push_back(clientFd);
				}
			} else if (pollFds[i].revents & POLLIN) {
				handleClientCommunication(pollFds[i].fd);
			}
		}
	}

	close(servSocket);
}

void validateArguments(char *port, char *password, int &portNumber, std::string &serverPassword) {
	portNumber = std::atoi(port);
	if (portNumber <= 0 || portNumber > 65535) {
		std::cerr << "Error: <port> must be a valid positive number between 1 and 65535." << std::endl;
		exit(0);
	}

	if (!password || std::strlen(password) == 0) {
		std::cerr << "Error: <password> is needed." << std::endl;
		exit(0);
	}

	serverPassword = password;
}