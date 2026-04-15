#include "../inc/MimeType.hpp"

MimeType::MimeType()
{
	_mime_types[".avi"] = "video/x-msvideo";
	_mime_types[".avif"] = "image/avif";
	_mime_types[".aac"] = "audio/aac";
	_mime_types[".bmp"] = "image/bmp";
	_mime_types[".csh"] = "application/x-csh";
	_mime_types[".css"] = "text/css";
	_mime_types[".csv"] = "text/csv";
	_mime_types[".doc"] = "application/msword";
	_mime_types[".gif"] = "image/gif";
	_mime_types[".gz"] = "application/x-gzip";
	_mime_types[".html"] = "text/html";
	_mime_types[".htm"] = "text/html";
	_mime_types[".ico"] = "image/x-icon";
	_mime_types[".ico"] = "image/x-icon";
	_mime_types[".js"] = "text/javascript";
	_mime_types[".json"] = "application/json";
	_mime_types[".jpg"] = "image/jpeg";
	_mime_types[".jpeg"] = "image/jpeg";
	_mime_types[".mp3"] = "audio/mp3";
	_mime_types[".mp4"] = "video/mp4";
	_mime_types[".php"] = "application/x-http-php";
	_mime_types[".pdf"] = "application/pdf";
	_mime_types[".png"] = "image/png";
	_mime_types[".sh"] = "application/x-sh";
	_mime_types[".tar"] = "application/x-tar";
	_mime_types[".txt"] = "text/plain";
	_mime_types[".wav"] = "audio/wav";
	_mime_types[".zip"] = "application/zip";
	_mime_types[".7z"] = "application/x-7z-compressed";
	_mime_types["default"] = "application/octet-stream"; // default value for other cases and unknown type
}

std::string	MimeType::getMimeType(const std::string extension)
{
	if (_mime_types.count(extension))
		return (_mime_types[extension]);
	return (_mime_types["default"]);
}
