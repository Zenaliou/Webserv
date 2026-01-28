#include "../inc/Server.hpp"

/*
 run() - Main server event loop

 This is the heart of the server. It uses poll() to monitor multiple
 file descriptors simultaneously (multiplexing).

 How it works:
 1. poll() blocks until one or more fds have events
 2. We iterate through all monitored fds
 3. If event is on server fd -> new client connecting (acceptClient)
 4. If event is on client fd -> client sent data (handleClient)

 poll() parameters:
   - &pollfds[0]: array of pollfd structures to monitor
   - pollfds.size(): number of fds to monitor
   - -1: wait indefinitely (no timeout)
*/
void ServerSocket::run()
{
	std::cout << "Server running on port..." << std::endl;
	
	while (true)
	{
		// Wait for events on any monitored file descriptor
		int ret = poll(&pollfds[0], pollfds.size(), -1);
		if (ret < 0)
		{
			std::cerr << "poll() error" << std::endl;
			break;
		}

		// Check each fd for events
		for (size_t i = 0; i < pollfds.size(); i++)
		{
			if (pollfds[i].revents == 0) // no event on this fd
				continue;

			int fd = pollfds[i].fd;

			if (fd == getFd()) // event on server socket = new connection
			{
				acceptClient();
			}
			else // event on client socket = data to read
			{
				Client *client = clients[fd];
				int status = client->handleClient();
				
				if (status <= 0) // 0 = disconnected, -1 = error
					removeClient(fd);
			}
		}
	}
}

/*
** acceptClient() - Accept a new incoming connection
**
** Called when poll() detects activity on the server socket.
**
** Steps:
** 1. accept() creates a NEW socket for this specific client
** 2. Set the new socket to non-blocking mode
** 3. Create a Client object to manage this connection
** 4. Add the client fd to poll() monitoring list
**
** accept() returns a NEW file descriptor for the client.
** The server socket continues listening for more connections.
*/
void ServerSocket::acceptClient()
{
	// accept() extracts first connection from queue, creates new socket
	int client_fd = accept(getFd(), NULL, NULL);
	if (client_fd < 0)
	{
		// EAGAIN/EWOULDBLOCK = no pending connections (normal in non-blocking)
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			std::cerr << "accept() failed" << std::endl;
		return;
	}

	// Set non-blocking so recv() won't block if no data available
	setNonBlocking(client_fd);
	
	// Create client object and store it
	Client *client = new Client(client_fd);
	clients[client_fd] = client;
	
	Pollfd p;
	p.fd = client_fd;
	p.events = POLLIN;
	pollfds.push_back(p);

	std::cout << "New client connected: fd " << client_fd << std::endl;
}

/*
** removeClient() - Clean up a disconnected client
**
** Called when handleClient() returns 0 (graceful disconnect) or -1 (error).
**
** Steps:
** 1. Remove fd from pollfds vector (stop monitoring)
** 2. Delete the Client object (this also closes the socket)
** 3. Remove from clients map
**
** Memory management: delete calls Client destructor which calls closeConnection()
*/
void ServerSocket::removeClient(int fd)
{
	// remove from pollfds
	for (size_t i = 0; i < pollfds.size(); i++)
	{
		if (pollfds[i].fd == fd)
		{
			pollfds.erase(pollfds.begin() + i);
			break;
		}
	}

	// delete client object
	std::map<int, Client*>::iterator it = clients.find(fd);
	if (it != clients.end())
	{
		delete it->second;
		clients.erase(it);
	}

	std::cout << "Client disconnected: fd " << fd << std::endl;
}