#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <string>
#include <map>

class Response
{
	private:
		int _statusCode;
		std::string _statusMessage;
		std::map<std::string, std::string> _responseHeaders;
		std::string _responseBody;
	public:
		Response(int statusCode, const std::string &statusMessage);
		~Response();
};

#endif