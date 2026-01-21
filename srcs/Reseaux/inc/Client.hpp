#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Librairy.hpp"
#include "../Webserv.hpp"

class Client : public Webserv // not finished yet
{
	public:
		int 		fd;
		std::string buffer_in;
		std::string buffer_out;
		size_t		offset;

		Client(int fd) : fd (fd), offset(0) {} ;
		
		void acceptClient();
		void handleClient();
};

#endif