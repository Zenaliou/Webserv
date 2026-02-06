#ifndef CGI_HPP
	#define CGI_HPP
	#include <iostream>
	#include <string>
	#include <map>

	class	CGI
	{
		private:
			std::string	_method;
			std::string	_interpreter;
			std::string	_scriptPath;
			std::string	_queryString;
			std::string	_body;
			std::map<std::string, std::string>	_headers;

			char** buildEnv();
			void freeEnv(char** env);
		public:
			CGI();
			~CGI();

			void	setMethod(const std::string &method);
			void	setInterpreter(const std::string &interpreter);
			void	setScriptPath(const std::string &scriptpath);
			void	setQueryString(const std::string &querystring);
			void	setBody(const std::string &body);
			void	setHeaders(const std::map<std::string, std::string> &headers);

			std::string execute();
	};

#endif 	
