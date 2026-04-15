#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

#include "config.hpp"
#include <string>
#include <map>
#include <sstream>

class RequestParser{
	private:
	std::string _method;
	std::string _uri;
	std::string _httpVersion;

	std::map<std::string, std::string> _headers;
	std::string _body;
	size_t _expectedBodyLength;

	Server _server;
	Location _location;

	int _errorCode;

	const Server &selectServer(const config &config, int port, const std::string &host);
	const Location &selectLocation(const Server &server, const std::string &path);

	public:
	RequestParser();
	~RequestParser();

	const std::string &getMethod() const;
	const std::string &getUri() const;
	const std::string &getHttpVersion() const;
	const std::string &getBody() const;
	const Server &getServer() const;
	const Location &getLocation() const;
	int getErrorCode() const;

	void setMethod(const std::string &method);
	void setUri(const std::string &uri);
	void setBody(const std::string &body);

	std::string getHeaderValue(const std::string& name) const;

	std::string toLower(const std::string &str) const;

	void parseRequest(std::string &buffer, const config &config, int port);
	bool parseRequestLine(const std::string &line);
	void parseHeader(const std::string &line);

	size_t getContentLength() const;
	bool hasHeader(const std::string &name) const;
	bool isChunked() const;

	bool parseChunkBody(const std::string &data, size_t bodyStart);

};

#endif
