#include "../inc/IRC.hpp"

std::string check_password(std::string password) {
	if (password.length() < 4) {
		std::cout << "Error: Password must be 4 characters long at least" << std::endl;
		exit (0);
	}
	return (password);
}

int check_port(std::string port) {
	if (port.empty()) {
		std::cout << "Error: Port can't be empty" << std::endl;
		exit (0);
	}
	if (port.length() > 7) {
		std::cout << "Error: Port must be only digits from 49152 to 65535" << std::endl;
		exit (0);
	}
	int p = 0;
	for (std::size_t i = 0; i < port.length(); i++) {
		if (port[i] < '0' || port[i] > '9') {
			std::cout << "Error: Port must be only digits from 49152 to 65535" << std::endl;
			exit (0);
		}
		p = std::atoi(port.c_str());
		if (p < 49152 || p > 65535) {
			std::cout << "Error: Port must be only digits from 49152 to 65535" << std::endl;
			exit (0);
		}
	}
	return (p);
}

t_data parse_args(std::string password, std::string port) {
	t_data data;

	data.password = check_password(password);
	data.port = check_port(port);
	
	return (data);
}