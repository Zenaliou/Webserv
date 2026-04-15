#ifndef REQUESTHANDLER_HPP
	#define REQUESTHANDLER_HPP

#include "RequestParser.hpp"
#include "Response.hpp"
#include "MimeType.hpp"
#include "config.hpp"
#include "CGI.hpp"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <algorithm>

	class	RequestHandler
	{
		public:
			RequestHandler();
			~RequestHandler();
			void handleRequest(const RequestParser &request, Response &response, const config &config, int port);
			std::string getTimeStamp() const;
		private:
			MimeType	_mimetypes;
			const Server &selectServer(const config &config, int port, const std::string &host);
			const Location &selectLocation(const Server &server, const std::string &path);
			void handleGet(const RequestParser &request, Response &response, const Location &location);
			void handlePost(const RequestParser &request, Response &response, const Location &location);
			void handleDelete(const RequestParser &request, Response &response, const Location &location);
			void setErrorResponse(Response &response, int code, const std::string &reason_phrase, const std::string &file);
			void setErrorResponseTemp(Response &response, int code, const std::string &reason_phrase, const std::string &body);
			void handleRequestError(Response &response, int errorCode);
	};

	std::string loadFile(const std::string &path);

#endif
