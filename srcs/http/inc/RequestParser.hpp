#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

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

	int _errorCode;

	public:
	RequestParser();
	~RequestParser();

	const std::string &getMethod() const;
	const std::string &getUri() const;
	const std::string &getHttpVersion() const;
	const std::string &getBody() const;
	int getErrorCode() const;

	void setMethod(const std::string &method);
	void setUri(const std::string &uri);
	void setBody(const std::string &body);

	std::string getHeaderValue(const std::string& name) const;

	std::string toLower(const std::string &str) const;

	void parseRequest(std::string &buffer);
	bool parseRequestLine(const std::string &line);
	void parseHeader(const std::string &line);

	size_t getContentLength() const;
	bool hasHeader(const std::string &name) const;
	bool isChunked() const;

	bool parseChunkBody(const std::string &data, size_t bodyStart);

};

#endif
