#include "CGI.hpp"

CGI::CGI() {}
CGI::~CGI() {}

char**	CGI::buildEnv()
{
	std::vector<std::string> env;

	env.push_back("REQUEST_METHOD=" + _method);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SCRIPT_FILENAME=" + _scriptPath);
	env.push_back("QUERY_STRING=" + _queryString);
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");

	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		if (it->first == "Content_Length")
			env.push_back("CONTENT_LENGTH=" + it->second);
		if (it->first == "Content_Type")
			env.push_back("CONTENT_TYPE=" + it->second);
		else
			env.push_back("HTTP_" + it->first + "=" + it->second);
	}

	char	**envp = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); i++)
	{
		envp[i] = new char[env[i].size() + 1];
		std::strcpy(envp[i], env[i].c_str());
	}

	envp[env.size()] = NULL;
	return (envp);
}

void	freeEnv(char** env)
{
	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
}


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

