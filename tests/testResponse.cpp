#include "../srcs/http/HttpRequest.hpp"
#include "../srcs/http/Response.hpp"
#include "../srcs/http/RequestHandler.hpp"
#include <iostream>
#include <fstream>

// int	main()
// {
// 	try
// 	{
// 		Response rep(200, "OK");
// 		std::ifstream	inFile;
// 		inFile.open("../../www/html/test.html");

// 		std::stringstream	strstream;
// 		strstream << inFile.rdbuf();
// 		std::string str = strstream.str();
// 		rep.setBody(str);
// 		rep.displayResponseHeaders();
// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << " -> Exception:" << e.what() << std::endl;
// 	}

// 	return (0);
// }

int main()
{
	std::string rawRequest =
		"GET /html/index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";

	HttpRequest request;
	request.parseRequest(rawRequest);
	if (request.isComplete())
	{

		Response response;
		std::cout << "get all request: " << request.getMethod() << std::endl;
		std::cout << request.getUri() << std::endl;
		std::cout << request.getHttpVersion() << std::endl;
		std::cout << request.getBody() << std::endl;

		RequestHandler handler;
		handler.handleRequest(request, response);
		std::cout << std::endl;
		std::cout << response.generateResponse() << std::endl;
	}
	return (0);
}
