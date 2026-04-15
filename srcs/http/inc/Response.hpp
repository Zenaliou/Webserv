#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <string>
#include <map>
#include <ctime>
#include <sstream>

class Response
{
	private:
		unsigned int _statusCode;
		std::string _reasonPhrase;
		std::map<std::string, std::string> _responseHeaders;
		std::string _responseBody;
	public:
		Response();
		Response(unsigned int statusCode, const std::string &reasonPhrase);
		~Response();

		// getters
		unsigned int	getStatusCode() const;
		std::string		getStatusMessage()const;
		std::map<std::string, std::string>	getResponseHeaders() const;
		std::string		getResponseBody() const;

		// setters
		void setStatus(unsigned int statusCode, const std::string &reasonPhrase);
		void setBody(const std::string &body);

		// methods
		std::string generateResponse() const;

		// utils
		std::string getStatusLine() const;
		void addHeader(const std::string &key, const std::string &value);
		std::string getDate() const;
		void displayResponseHeaders() const;
		std::string intToString(const unsigned int nb) const;

	class	InvalidStatusCodeException : public std::exception
	{
		public:
		virtual const char *what() const throw();
	};
};

#endif
