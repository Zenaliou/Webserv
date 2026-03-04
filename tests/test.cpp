#include "../srcs/http/Response.hpp"

int	main()
{
	Response res(200, "OK");
	res.addHeader("Content-Type", "text/html");
	res.addHeader("Connection", "close");
	res.getResponseHeaders();
	return 0;
}