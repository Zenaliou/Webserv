#include "Response.hpp"

Response::Response(int statusCode, const std::string &statusMessage)
	: _statusCode(statusCode), _statusMessage(statusMessage)
{
	_responseHeaders["Date"] = getDate();
}

Response::~Response()
{}

std::string Response::getStatusLine() const
{
	return "HTTP/1.1 " + std::to_string(_statusCode) + " " + _statusMessage + "\r\n";
}

void Response::addHeader(const std::string &key, const std::string &value)
{
	_responseHeaders[key] = value;
}

std::string Response::getDate() const
{
	time_t now = time(0);
	struct tm datetime;
	char buf[50];

	datetime = *gmtime(&now);
	// Format: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
	strftime(buf, sizeof(buf), "%a, %d %b %Y %X GMT", &datetime);
	return std::string(buf);
}

void Response::getResponseHeaders() const
{
	std::cout << getStatusLine();
	for (std::map<std::string, std::string>::const_iterator it = _responseHeaders.begin(); it != _responseHeaders.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << "\r\n";
	}
	std::cout << "\r\n";
	std::cout << _responseBody;
}