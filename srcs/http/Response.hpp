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

		std::string getStatusLine() const;
		void addHeader(const std::string &key, const std::string &value);
		// void setBody(const std::string &body);
		std::string getDate() const;

		void getResponseHeaders() const;
};

#endif