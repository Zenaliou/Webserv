#ifndef POLLFD_HPP
#define POLLFD_HPP

#include "../Librairy.hpp"

class Pollfd : public pollfd
{
	public:
		int		fd;
		short	events;
		short	revents;
};

#endif