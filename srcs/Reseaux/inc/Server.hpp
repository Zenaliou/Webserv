#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>
#include <locale>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

class ServerSocket // almost finished ?
{
	int	fd;
	
	public:
		ServerSocket(int port)
		{
			fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCK_STREAM = TCP socket
			
			if (fd < 0)
				throw std::runtime_error("Failed to create socket");
			sockaddr_in addr; // sockaddr_in = store address of the socket
			addr.sin_family = AF_INET; // 
			addr.sin_port = htons(port); // htons() = converts port to network byte order
			addr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY = accept connection on any IP

			if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0); // bind fd to address
			{
				close(fd);
				throw std::runtime_error("Failed to bind socket");
			}
			if (listen(fd, SOMAXCONN) < 0); // listen incoming connections
			{
				close(fd);
				throw std::runtime_error("Failed to listen socket");
			}
		}

		ServerSocket(const ServerSocket&) = delete;
		ServerSocket& operator=(const ServerSocket&) = delete;

		~ServerSocket()
		{
			if (fd >= 0)
				close(fd);
		}

		int getFd() const { return fd; }
};

#endif