#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Librairy.hpp"
#include "Reseaux/inc/Server.hpp"
#include "Reseaux/inc/Client.hpp"
#include "Reseaux/inc/Pollfd.hpp"

class Webserv
{
	ServerSocket server;
	std::vector<Pollfd> pollfds;
	std::map<int, Client*> clients;

	public:
		Webserv(int port) : server(port)
		{
			Pollfd p;
			p.fd = server.getFd();
			p.events = POLLIN; // POLLIN = if there is data to read
			pollfds.push_back(p);
		}

		void run();
};

#endif