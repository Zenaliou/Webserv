#include "CGI.hpp"
#include <string>

int	main()
{
	CGI	cgi;
	cgi.setMethod("GET");
	cgi.setInterpreter("/usr/bin/python3");
	cgi.setScriptPath("../../www/cgi-bin/hello.py");
	cgi.setQueryString("");
	cgi.setBody("");

	try
	{
		std::string str = cgi.execute();
		std::cout << str << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << " ->Exception: " << e.what() << std::endl;
	}
	return (0);
}
