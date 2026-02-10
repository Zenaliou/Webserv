#include "RequestHandler.hpp"

RequestHandler::RequestHandler() {}

RequestHandler::~RequestHandler() {}

void RequestHandler::handleRequest(const HttpRequest &request, Response &response)
{
	const std::string &method = request.getMethod();
	if (method == "GET")
		handleGet(request, response);
	else if (method == "POST")
		handlePost(request, response);
	else if (method == "DELETE")
		handleDelete(request, response);
	else
	{
		response.setStatus(405, "Method Not Allowed");
		response.setBody("405 Method Not Allowed\n");
		response.addHeader("Content-Type", "text/plain");
	}
}
