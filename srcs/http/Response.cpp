#include "Response.hpp"

Response::Response() : _statusCode(200), _reasonPhrase("OK")
{
	_responseHeaders["Date"] = getDate();
}

Response::Response(unsigned int statusCode, const std::string &reasonPhrase)
	: _statusCode(statusCode), _reasonPhrase(reasonPhrase)
{
	_responseHeaders["Date"] = getDate();
	if (_statusCode < 100 || _statusCode > 599)
		throw(InvalidStatusCodeException());
}

Response::~Response(){}

unsigned int	Response::getStatusCode() const
{
	return (_statusCode);
}

// getters
std::string		Response::getStatusMessage() const
{
	return (_reasonPhrase);
}

std::map<std::string, std::string>	Response::getResponseHeaders() const
{
	return (_responseHeaders);
}

std::string		Response::getResponseBody() const
{
	return (_responseBody);
}

// setters
void Response::setStatus(unsigned int statusCode, const std::string &reasonPhrase)
{
	if (_statusCode < 100 || _statusCode > 599)
		throw(InvalidStatusCodeException());
	else
	{
		_statusCode = statusCode;
		_reasonPhrase = reasonPhrase;
	}
}

void Response::setBody(const std::string &body)
{
	_responseBody = body;
	addHeader("Content-Length", std::to_string(_responseBody.size()));
}

// methods
std::string Response::generateResponse() const
{
	std::string	response = getStatusLine();
	for (std::map<std::string, std::string>::const_iterator it = _responseHeaders.begin(); it != _responseHeaders.end(); ++it)
	{
		response += it->first + ": " + it->second + "\r\n";
	}
	response += "\r\n" + _responseBody;
	return (response);
}

// utils
std::string Response::getStatusLine() const
{
	return ("HTTP/1.1 " + std::to_string(_statusCode) + " " + _reasonPhrase + "\r\n");
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

void Response::displayResponseHeaders() const
{
	std::cout << getStatusLine();
	for (std::map<std::string, std::string>::const_iterator it = _responseHeaders.begin(); it != _responseHeaders.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << "\r\n";
	}
	std::cout << "\r\n";
	std::cout << _responseBody;
}

const char *Response::InvalidStatusCodeException::what() const throw()
{
	return ("Invalid Status Code");
}
