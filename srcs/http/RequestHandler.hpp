#ifndef REQUESTHANDLER_HPP
	#define REQUESTHANDLER_HPP

#include "HttpRequest.hpp"
#include "Response.hpp"
#include "MimeType.hpp"
#include "config.hpp"
#include "CGI.hpp"
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <algorithm>

	class	RequestHandler
	{
		public:
			RequestHandler();
			~RequestHandler();
			void handleRequest(const HttpRequest &request, Response &response, const config &config, int port);

		private:
			MimeType	_mimetypes;
			const Server &selectServer(const config &config, int port, const std::string &host);
			const Location &selectLocation(const Server &server, const std::string &path);
			void handleGet(const HttpRequest &request, Response &response, const Server &server);
			void handlePost(const HttpRequest &request, Response &response, const Server &server);
			// void handleDelete(const HttpRequest &request, Response &response, const config &config);

	};

#endif
