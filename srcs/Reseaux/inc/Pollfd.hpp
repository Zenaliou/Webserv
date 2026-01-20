#ifndef POLLFD_HPP
#define POLLFD_HPP

#include "../Librairy.hpp"

class Pollfd
{
	public:
		int		fd;
		short	events;
		short	revents;
};

#endif