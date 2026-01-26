#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"
#include "Reseaux/inc/Client.hpp"

class ServerSocket : public Webserv
{
	public:
		ServerSocket(int port) : Webserv(port)
		{
			sockaddr_in addr;
			addr.sin_family = AF_INET;          // IPv4
			addr.sin_port = htons(port);        // convert port to network byte order
			addr.sin_addr.s_addr = INADDR_ANY;  // accept connections on any interface

			int opt = 1;
			setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // allow port reuse

			if (bind(getFd(), (struct sockaddr*)&addr, sizeof(addr)) < 0) // bind socket to address
				throw std::runtime_error("Failed to bind socket");
			if (listen(getFd(), SOMAXCONN) < 0) // start listening for incoming connections
				throw std::runtime_error("Failed to listen socket");
			
			setNonBlocking(getFd()); // set socket to non-blocking mode
		}

		void run();          // main server loop
		void acceptClient(); // accept a new client connection
		void removeClient(int fd); // remove a disconnected client

	private:
		ServerSocket(const ServerSocket&);
		ServerSocket& operator=(const ServerSocket&);
};

#endif