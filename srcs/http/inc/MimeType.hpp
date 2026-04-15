#ifndef MIMETYPE_HPP
	#define MIMETYPE_HPP
	#include <iostream>
	#include <map>

	class	MimeType
	{
		private:
			std::map<std::string, std::string> _mime_types;
		public:
			MimeType();
			std::string	getMimeType(const std::string extension);
	};

#endif
