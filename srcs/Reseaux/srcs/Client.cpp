#include "../Webserv.hpp"

void Client::acceptClient() // have to move it on server
{
	int client_fd = accept(getFd(), NULL, NULL);
	Pollfd p;

	setNonBlocking(client_fd); // function not created yet
	Client *client = new Client(client_fd);
	clients[client_fd] = client;
	
	p.fd = getFd();
	p.events = POLLIN;
	pollfds.push_back(p);
}

void Client::handleClient()
{
	char buffer[4096];
	ssize_t bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read > 0)
	{
		buffer[bytes_read] = '\0';
		buffer_in += buffer;
		
		if (buffer_in.find("\r\n\r\n") != std::string::npos) // \r\n\r\n mean request is completed
		{
			buffer_out = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello World!\n"
			send(fd , buffer_out.c_str(), buffer_out.size(), 0)
			buffer_in.clear();
		}
	}

	if (bytes_read == 0)
	{
		
	}
}