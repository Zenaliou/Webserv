#include "Webserv.hpp"

void Webserv::run()
{
	while (true)
	{
		poll(&pollfds[0], pollfds.size(), -1);

		for (size_t i = 0; i < pollfds.size(); i++)
		{
			if (pollfds[i].revents == 0)
				continue;
			int fd = pollfds[i].fd;

			if (fd == getFd())
				acceptClient();
			else
				handleClient();
		}
	}
}