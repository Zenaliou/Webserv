#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Librairy.hpp"
#include "Reseaux/inc/Server.hpp"
#include "Reseaux/inc/Client.hpp"
#include "Reseaux/inc/Pollfd.hpp"

class Webserv
{
	int fd;
	

	public:
	// ServerSocket server;
	std::vector<Pollfd> pollfds;
	std::map<int, Client*> clients;

	public:
		Webserv();
		Webserv(int port)
		{
			fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCK_STREAM = TCP socket
			
			if (fd < 0)
				throw std::runtime_error("Failed to create socket");
			sockaddr_in addr; // sockaddr_in = store address of the socket
			addr.sin_family = AF_INET; // 
			addr.sin_port = htons(port); // htons() = converts port to network byte order
			addr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY = accept connection on any IP

			Pollfd p;
			p.fd = getFd();
			p.events = POLLIN; // POLLIN = if there is data to read
			pollfds.push_back(p);
		}

		~Webserv()
		{
			if (fd >= 0)
				close(fd);
		}

		void run();
		int getFd() const { return fd; }
		void acceptClient();

};

#endif