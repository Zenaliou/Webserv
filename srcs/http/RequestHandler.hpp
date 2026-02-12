#ifndef REQUESTHANDLER_HPP
	#define REQUESTHANDLER_HPP

#include "HttpRequest.hpp"
#include "Response.hpp"
#include "MimeType.hpp"
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <sstream>

	class	RequestHandler
	{
		public:
			RequestHandler();
			~RequestHandler();
			void handleRequest(const HttpRequest &request, Response &response);

		private:
			MimeType	_mimetypes;
			void handleGet(const HttpRequest &request, Response &response);
			// void handlePost(const HttpRequest &request, Response &response);
			// void handleDelete(const HttpRequest &request, Response &response);
	};

#endif
