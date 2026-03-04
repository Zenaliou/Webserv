#include "../inc/WebservManager.hpp"

volatile sig_atomic_t g_running = 1;

static void signalHandler(int sig)
{
	(void)sig;
	g_running = 0;
	std::cout << "\nShutting down..." << std::endl;
}

WebservManager::WebservManager(const config &cfg) : _config(&cfg)
{
	const std::vector<Server> &servers = cfg.getServers();
	
	// Create a ServerSocket for each server configuration
	for (size_t i = 0; i < servers.size(); ++i)
	{
		int port = servers[i].getPort();
		std::cout << "Initializing server on port " << port << "..." << std::endl;
		
		ServerSocket *serverSocket = new ServerSocket(port, cfg);
		_servers.push_back(serverSocket);
		
		// Add server fd to pollfds
		Pollfd p;
		p.fd = serverSocket->getFd();
		p.events = POLLIN;
		_pollfds.push_back(p);
		
		// Map server fd to ServerSocket
		_serverFdMap[serverSocket->getFd()] = serverSocket;
	}
	
	std::cout << "WebservManager initialized with " << _servers.size() << " server(s)" << std::endl;
}

WebservManager::~WebservManager()
{
	// Clean up all clients
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		delete it->second;
	}
	_clients.clear();
	
	// Clean up all servers
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		delete _servers[i];
	}
	_servers.clear();
}

void WebservManager::run()
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	
	std::cout << "WebservManager starting main event loop..." << std::endl;
	
	while (g_running)
	{
		// Wait for events on any monitored file descriptor
		int ret = poll(&_pollfds[0], _pollfds.size(), -1);
		if (ret < 0)
		{
			if (!g_running) // Interrupted by signal
				break;
			std::cerr << "poll() error" << std::endl;
			break;
		}
		
		// Check each fd for events
		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents == 0) // no event on this fd
				continue;
			
			int fd = _pollfds[i].fd;
			
			// Check if it's a server socket (new connection)
			if (_serverFdMap.find(fd) != _serverFdMap.end())
			{
				ServerSocket *server = _serverFdMap[fd];
				acceptClient(server);
			}
			else // It's a client socket (data to read)
			{
				handleClient(fd);
			}
		}
	}
}

void WebservManager::acceptClient(ServerSocket *server)
{
	// accept() creates a NEW socket for this specific client
	int client_fd = accept(server->getFd(), NULL, NULL);
	if (client_fd < 0)
	{
		// EAGAIN/EWOULDBLOCK = no pending connections (normal in non-blocking)
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			std::cerr << "accept() failed" << std::endl;
		return;
	}
	
	// Set non-blocking so recv() won't block if no data available
	Webserv::setNonBlocking(client_fd);
	
	// Create client object and store it
	Client *client = new Client(client_fd, _config, server->getPort());
	_clients[client_fd] = client;
	
	// Add client fd to pollfds
	Pollfd p;
	p.fd = client_fd;
	p.events = POLLIN;
	_pollfds.push_back(p);
	
	std::cout << "New client connected on port " << server->getPort() 
	          << ": fd " << client_fd << std::endl;
}

void WebservManager::handleClient(int client_fd)
{
	Client *client = _clients[client_fd];
	int status = client->handleClient();
	
	if (status <= 0) // 0 = disconnected, -1 = error
		removeClient(client_fd);
}

void WebservManager::removeClient(int fd)
{
	// Remove from pollfds
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].fd == fd)
		{
			_pollfds.erase(_pollfds.begin() + i);
			break;
		}
	}
	
	// Delete client object
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end())
	{
		delete it->second;
		_clients.erase(it);
	}
	
	std::cout << "Client disconnected: fd " << fd << std::endl;
}
