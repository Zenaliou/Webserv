#include "server/inc/Server.hpp"

int main()
{
	try
	{
		ServerSocket server(1024);
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
