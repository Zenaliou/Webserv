#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <sstream>

class HttpRequest{
	private:
	std::string _method;
	std::string _uri;
	std::string _httpVersion;

	std::map<std::string, std::string> _headers;
	std::string _body;
	size_t _expectedBodyLength;

	int _errorCode;
	bool _requestComplete;

	public:
	HttpRequest();
	//HttpRequest(const HttpRequest& other);
	~HttpRequest();

	//HttpRequest& operator= (const HttpRequest& other);

	bool isComplete() const;

	const std::string &getMethod() const;
	const std::string &getUri() const;
	const std::string &getHttpVersion() const;
	const std::string &getBody() const;

	std::string getHeaderValue(const std::string& name) const;

	std::string toLower(const std::string &str) const;

	bool parseRequest(const std::string &data);

	bool parseRequestLine(const std::string &line);
	void parseHeader(const std::string &line);


	size_t getContentLength() const;
	bool hasHeader(const std::string &name) const;
	bool isChunked() const;

	bool parseChunkBody(const std::string &data, size_t bodyStart);

};

#endif