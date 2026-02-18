#include "server/inc/WebservManager.hpp"
#include "http/config.hpp"

int main(int argc, char **argv)
{
	try
	{
		// Parse configuration file
		std::string configFile = "conf/default_test.conf";
		if (argc > 1)
			configFile = argv[1];
		
		config cfg(configFile);
		const std::vector<Server> &servers = cfg.getServers();
		
		if (servers.empty())
			throw std::runtime_error("No server configuration found");
		
		// Create WebservManager to handle all servers
		WebservManager manager(cfg);
		manager.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
