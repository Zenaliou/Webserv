#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Librairy.hpp"
#include "../../http/HttpRequest.hpp"
#include "../../http/Response.hpp"
#include "../../http/RequestHandler.hpp"
#include "../../http/config.hpp"

class Client
{
	public:
		int 		fd;
		std::string buffer_in;
		std::string buffer_out;
		size_t		offset;

		HttpRequest request;

		Client(int fd, const config *cfg, int port) : fd(fd), offset(0), _config(cfg), _port(port) {} ;
		~Client() { closeConnection(); }
		
		int handleClient();
		void closeConnection();
		
	private:
		const config *_config;
		int _port;
};

#endif