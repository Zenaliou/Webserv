#ifndef CGI_HPP
	#define CGI_HPP
	#include <iostream>
	#include <cstring>
	#include <cstdlib>
	#include <string>
	#include <map>
	#include <vector>
	#include <unistd.h>
	#include <sys/wait.h>

	class	CGI
	{
		private:
			std::string	_method;
			std::string	_interpreter; // exec de python
			std::string	_scriptPath; // path du fichier a exec
			std::string	_body;
			std::map<std::string, std::string>	_headers;

			char** buildEnv();
			char** buildArgv();
			void freeEnv(char** env);
			void freeArgv(char** argv);
		public:
			CGI();
			~CGI();

			// setters to use in Response
			void	setMethod(const std::string &method);
			void	setInterpreter(const std::string &interpreter);
			void	setScriptPath(const std::string &scriptpath);
			void	setBody(const std::string &body);
			void	setHeaders(const std::map<std::string, std::string> &headers);

			std::string execute();
	};

#endif 	
