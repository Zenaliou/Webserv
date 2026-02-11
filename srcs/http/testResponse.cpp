#include "Response.hpp"
#include <fstream>

int	main()
{
	try
	{
		Response rep(2, "Test");
		rep.displayResponseHeaders();
	}
	catch(const std::exception& e)
	{
		std::cerr << " -> Exception: " << e.what() << std::endl;
	}
	try
	{
		Response rep(200, "OK");
		rep.displayResponseHeaders();
	}
	catch(const std::exception& e)
	{
		std::cerr << " -> Exception:" << e.what() << std::endl;
	}
	std::cout << "\n----------------\n" << std::endl;
	try
	{
		Response rep(200, "OK");
		std::ifstream	inFile;
		inFile.open("../../www/html/test.html");

		std::stringstream	strstream;
		strstream << inFile.rdbuf();
		std::string str = strstream.str();
		rep.setBody(str);
		rep.displayResponseHeaders();
	}
	catch(const std::exception& e)
	{
		std::cerr << " -> Exception:" << e.what() << std::endl;
	}
	
	return (0);
}
