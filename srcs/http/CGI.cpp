#include "CGI.hpp"

CGI::CGI() {}
CGI::~CGI() {}

void	CGI::setMethod(const std::string &method)
{
	this->_method = method;
}

void	CGI::setInterpreter(const std::string &interpreter)
{
	this->_interpreter = interpreter;
}

void	CGI::setScriptPath(const std::string &scriptpath)
{
	this->_scriptPath = scriptpath;
}

void	CGI::setQueryString(const std::string &querystring)
{
	this->_queryString = querystring;
}

void	CGI::setBody(const std::string &body)
{
	this->_body = body;
}

void	CGI::setHeaders(const std::map<std::string, std::string> &headers)
{
	this->_headers = headers;
}

