#include "RequestHandler.hpp"

RequestHandler::RequestHandler() {}

RequestHandler::~RequestHandler() {}

void RequestHandler::handleRequest(const HttpRequest &request, Response &response)
{
	const std::string &method = request.getMethod();
	if (method == "GET")
		handleGet(request, response);
	else if (method == "POST")
		handlePost(request, response);
	else if (method == "DELETE")
		handleDelete(request, response);
	else
	{
		response.setStatus(405, "Method Not Allowed");
		response.setBody("405 Method Not Allowed\n");
		response.addHeader("Allow", "GET, POST, DELETE");
	}
}

void RequestHandler::handleGet(const HttpRequest &request, Response &response)
{
	std::string root = "./www"; // dossier racine serveur, a recup sur .config
	std::string path = request.getUri();

	if (path == "/")
		path = "/index.html"; // path par defaut

	std::string fullPath = root + path;

	struct stat fileStat;

	if (stat(fullPath.c_str(), &fileStat) != 0)
	{
		response.setStatus(404, "Not Found");
		response.setBody("404 Not Found\n");
		response.addHeader("Content-Type", "text/plain");
		return;
	}

	// Si c’est un dossier
	if (S_ISDIR(fileStat.st_mode))
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

	// Détection MIME
	size_t dotPos = fullPath.find_last_of('.');
	std::string extension;
	if (dotPos != std::string::npos)
		extension = fullPath.substr(dotPos + 1);
	std::string mime = _mimetypes.getMimeType(extension);
	response.addHeader("Content-Type", mime);
}

// void RequestHandler::handlePost(const HttpRequest &request, Response &response)
// {

// }

// void RequestHandler::handleDelete(const HttpRequest &request, Response &response)
// {

// }

