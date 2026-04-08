#ifndef WEBSERVMANAGER_HPP
#define WEBSERVMANAGER_HPP

#include "Server.hpp"
#include "../../http/config.hpp"
#include <vector>

/*
 WebservManager - Manages multiple server sockets with a single poll() loop
 
 This class allows running multiple servers (different ports) simultaneously
 by monitoring all server and client file descriptors in one poll() call.
*/
class WebservManager
{
	public:
		WebservManager(const config &cfg);
		~WebservManager();
		
		void run();
		
	private:
		const config *_config;
		std::vector<ServerSocket*> _servers;
		std::vector<Pollfd> _pollfds;
		std::map<int, Client*> _clients;
		std::map<int, ServerSocket*> _serverFdMap;
		std::map <int, bool> _port;
		
		void acceptClient(ServerSocket *server);
		void handleClient(int client_fd);
		void removeClient(int fd);
		
		WebservManager(const WebservManager&);
		WebservManager& operator=(const WebservManager&);
};

#endif
