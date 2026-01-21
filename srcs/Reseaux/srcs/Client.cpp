#include "../Webserv.hpp"

void Client::acceptClient()
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
	
}