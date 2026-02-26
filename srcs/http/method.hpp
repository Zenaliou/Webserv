#ifndef METHOD_HPP
# define METHOD_HPP

#include <string>
#include <sys/stat.h>//pour stat
#include <unistd.h>//pour unlink
#include "config.hpp"
#include "HttpRequest.hpp"
#include "Response.hpp"

class method{
	private:

	public:
	method();
	//method(const method& other);
	~method();

	//method& operator= (const method& other);

	void handleDelete(const HttpRequest &request, Response &response, const Location& location);
};

#endif