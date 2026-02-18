#include "RequestHandler.hpp"

RequestHandler::RequestHandler() {}

RequestHandler::~RequestHandler() {}

void RequestHandler::handleRequest(const HttpRequest &request, Response &response, const config &config)
{
	const std::string &method = request.getMethod();
	if (method == "GET")
		handleGet(request, response, config);
	// else if (method == "POST")
	// 	handlePost(request, response, config);
	// else if (method == "DELETE")
	// 	handleDelete(request, response, config);
	else
	{
		response.setStatus(405, "Method Not Allowed");
		response.setBody("405 Method Not Allowed\n");
		response.addHeader("Allow", "GET, POST, DELETE");
	}
}

void RequestHandler::handleGet(const HttpRequest &request, Response &response, const config &config)
{
	const std::vector<Server> &servers = config.getServers();
	std::string root;
	std::map<std::string, std::string> cgiExtensions;
	for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end(); ++it)
	{
		for (std::vector<Location>::const_iterator location_it = it->locations.begin(); location_it != it->locations.end(); ++location_it)
		{
			root = location_it->getRoot();  // root de chaque location
			cgiExtensions = location_it->getCgiExtensions();
		}
	}
	std::string path = request.getUri();
	if (path == "/")
		path = "/index.html"; // path par defaut

	std::string fullPath = root + path;

	// check extension
	size_t dotPos = fullPath.find_last_of('.');
	std::string extension;
	if (dotPos != std::string::npos)
		extension = fullPath.substr(dotPos);

	if (cgiExtensions.find(extension) != cgiExtensions.end())
	{
		// C'est un script CGI
		CGI cgi;
		cgi.setMethod("GET");
		cgi.setScriptPath(fullPath);
		cgi.setInterpreter(cgiExtensions[extension]); // Récupérer l'interpréteur

		std::string cgiOutput = cgi.execute();
		response.setStatus(200, "OK");
		response.setBody(cgiOutput);
		response.addHeader("Content-Type", "text/html");
	}
	struct stat fileStat;
	if (stat(fullPath.c_str(), &fileStat) != 0)
	{
		response.setStatus(404, "Not Found");
		response.setBody("404 Not Found\n"); // remplacer tous les 4xx par un fichier
		response.addHeader("Content-Type", "text/plain");
		return;
	}
	if (S_ISDIR(fileStat.st_mode)) // Si c’est un dossier
	{
		std::string indexPath = fullPath + "/index.html";
		if (stat(indexPath.c_str(), &fileStat) != 0)
		{
			response.setStatus(403, "Forbidden");
			response.setBody("403 Directory access forbidden\n");
			response.addHeader("Content-Type", "text/plain");
			return;
		}
		fullPath = indexPath;
	}
	// Déterminer si c'est un script CGI
	// Lecture fichier
	std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
	if (!file)
	{
		response.setStatus(403, "Forbidden");
		response.setBody("403 Cannot open file\n");
		response.addHeader("Content-Type", "text/plain");
		return;
	}
	std::ostringstream buffer;
	buffer << file.rdbuf();
	std::string body = buffer.str();

	response.setStatus(200, "OK");
	response.setBody(body);

	// Detection MIME
	std::string mime = _mimetypes.getMimeType(extension);
	response.addHeader("Content-Type", mime);
}

// void RequestHandler::handlePost(const HttpRequest &request, Response &response, const config &config)
// {
// 	const std::string &body = request.getBody();
// 	if (body.empty())
// 	{
// 		response.setStatus(400, "Bad Request");
// 		response.setBody("400 POST body is empty\n");
// 		response.addHeader("Content-Type", "text/plain");
// 		return;
// 	}
// }

// void RequestHandler::handleDelete(const HttpRequest &request, Response &response, const config &config)
// {

// }

