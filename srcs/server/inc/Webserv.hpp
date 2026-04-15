#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Librairy.hpp"
typedef struct pollfd Pollfd;
// #include "server/inc/Pollfd.hpp"

class Client; // forward declaration

/*
 Webserv - Base class for server socket management
 
 This class creates a TCP socket and manages the poll() mechanism.
 It stores all file descriptors to monitor (pollfds) and all connected clients.
 ServerSocket inherits from this class to add bind/listen functionality.
*/
class Webserv
{
	int fd; // server socket file descriptor
	

	public:
	std::vector<Pollfd> pollfds;      // list of file descriptors to monitor with poll()
	std::map<int, Client*> clients;   // map of connected clients (fd -> Client*)

	public:
		Webserv();
		
		/*
		 Constructor: Creates a TCP socket
		 @param port: port number (used by derived class ServerSocket)
		 
		 socket(AF_INET, SOCK_STREAM, 0):
		   - AF_INET: IPv4 protocol
		   - SOCK_STREAM: TCP (reliable, connection-oriented)
		   - 0: automatically choose protocol
		*/
		Webserv(int port)
		{
			(void)port; // used by derived class ServerSocket
			fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCK_STREAM = TCP socket
			
			if (fd < 0)
				throw std::runtime_error("Failed to create socket");

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

		int getFd() const { return fd; }
		static void setNonBlocking(int fd);

};

#endif
