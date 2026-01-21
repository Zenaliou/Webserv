#ifndef SERVER_HPP
#define SERVER_HPP

#include "../Librairy.hpp"

class ServerSocket : public Webserv// almost finished ?
{
	int	fd;
	
	public:
		ServerSocket(int port) : Webserv(port)
		{
			Webserv addr;
			if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) // bind fd to address
			{
				close(fd);
				throw std::runtime_error("Failed to bind socket");
			}
			if (listen(fd, SOMAXCONN) < 0) // listen incoming connections
			{
				close(fd);
				throw std::runtime_error("Failed to listen socket");
			}
		}

		ServerSocket(const ServerSocket&) = delete;
		ServerSocket& operator=(const ServerSocket&) = delete;

};

#endif