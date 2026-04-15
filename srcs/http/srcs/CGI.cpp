#include "../inc/CGI.hpp"

CGI::CGI() {}
CGI::~CGI() {}

char**	CGI::buildEnv()
{
	std::vector<std::string> env;

	env.push_back("REQUEST_METHOD=" + _method);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SCRIPT_FILENAME=" + _scriptPath);
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");

	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		if (it->first == "Content_Length")
			env.push_back("CONTENT_LENGTH=" + it->second);
		else if (it->first == "Content_Type")
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

char**	CGI::buildArgv()
{
	char	**argv = new char*[3];

	argv[0] = new char[_interpreter.size() + 1];
	std::strcpy(argv[0], _interpreter.c_str());
	argv[1] = new char[_scriptPath.size() + 1];
	std::strcpy(argv[1], _scriptPath.c_str());
	argv[2] = NULL;
	return (argv);
}

void	CGI::freeEnv(char** env)
{
	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
}

void	CGI::freeArgv(char** argv)
{
	for (size_t i = 0; argv[i]; i++)
		delete[] argv[i];
	delete[] argv;
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

void	CGI::setBody(const std::string &body)
{
	this->_body = body;
}

void	CGI::setHeaders(const std::map<std::string, std::string> &headers)
{
	this->_headers = headers;
}

std::string	CGI::execute()
{
	int	in_pipe[2];
	int	out_pipe[2];

	if (pipe(in_pipe) < 0 || pipe(out_pipe) < 0)
		throw std::runtime_error("pipe failed");
	pid_t	pid = fork();
	if (pid < 0)
		throw std::runtime_error("fork failed");

	if (pid == 0)
	{
		dup2(in_pipe[0], STDIN_FILENO);
		dup2(out_pipe[1], STDOUT_FILENO);
		close(in_pipe[1]);
		close(out_pipe[0]);

		char	**env = buildEnv();
		char	**argv = buildArgv();
		execve(argv[0], argv, env);
		freeEnv(env);
		freeArgv(argv);
		exit(1);
	}

	close(in_pipe[0]);
	close(out_pipe[1]);
	std::string	output;
	char	buffer[1024];
	ssize_t	bytes;

	if (!_body.empty())
		write(in_pipe[1], _body.c_str(), _body.size());
	close(in_pipe[1]);

	while ((bytes = read(out_pipe[0], buffer, sizeof(buffer))) > 0)
		output.append(buffer, bytes);
	close(out_pipe[0]);
	waitpid(pid, NULL, 0);
	return (output);
}
