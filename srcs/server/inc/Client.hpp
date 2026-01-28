#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Librairy.hpp"

class Client
{
	public:
		int 		fd;
		std::string buffer_in;
		std::string buffer_out;
		size_t		offset;

		Client(int fd) : fd (fd), offset(0) {} ;
		~Client() { closeConnection(); }
		
		int handleClient();
		void closeConnection();
};

#endif