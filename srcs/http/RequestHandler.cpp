#include "RequestHandler.hpp"

RequestHandler::RequestHandler() {}

RequestHandler::~RequestHandler() {}

const Server &RequestHandler::selectServer(const config &config, int port, const std::string &host)
{
	const std::vector<Server> &servers = config.getServers();
	for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end(); ++it)
	{
		if (it->getPort() == port && it->getHost() == host)
			return *it;
	}
	for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end(); ++it)
	{
		if (it->getPort() == port)
			return *it;
	}
	if (!servers.empty())
		return servers[0];
	throw std::runtime_error("No server configuration found");
}

const Location &RequestHandler::selectLocation(const Server &server, const std::string &path)
{
	const std::vector<Location> &locations = server.locations;
	size_t bestMatchIndex = 0;
	size_t longestMatchDepth = 0;
	bool foundMatch = false;

	// Chercher la location avec le chemin le plus precis correspondant au path
	for (size_t i = 0; i < locations.size(); ++i)
	{
		const std::string &locPath = locations[i].getPath();
		if (path.find(locPath) == 0)
		{
			size_t depth = std::count(locPath.begin(), locPath.end(), '/');
			if (depth > longestMatchDepth)
			{
				longestMatchDepth = depth;
				bestMatchIndex = i;
				foundMatch = true;
			}
		}
	}
	if (foundMatch)
		return locations[bestMatchIndex];
	for (size_t i = 0; i < locations.size(); ++i)
	{
		if (locations[i].getPath() == "/")
			return locations[i];
	}
	throw std::runtime_error("No location configuration found");
}

void RequestHandler::handleRequest(const HttpRequest &request, Response &response, const config &config, int port)
{
	try
	{
		const Server &server = selectServer(config, port, request.getHeaderValue("Host"));
		const std::string &method = request.getMethod();
		if (method == "GET")
			handleGet(request, response, server);
		else if (method == "POST")
			handlePost(request, response, server);
		// else if (method == "DELETE")
		// 	handleDelete(request, response, server);
		else
		{
			response.setStatus(405, "Method Not Allowed");
			response.setBody("405 Method Not Allowed\n");
			response.addHeader("Allow", "GET, POST, DELETE");
		}
	}
	catch (const std::exception &e)
	{
		response.setStatus(500, "Internal Server Error");
		response.setBody("500 Internal Server Error\n");
		response.addHeader("Content-Type", "text/plain");
	}
}

void RequestHandler::handleGet(const HttpRequest &request, Response &response, const Server &server)
{
	try
	{
		const Location &location = selectLocation(server, request.getUri());
		std::string root = location.getRoot();
		const std::map<std::string, std::string> &cgiExtensions = location.getCgiExtensions();

		std::string path = request.getUri();
		if (path == "/")
			path = "/index.html";

		std::string fullPath = root + path;

		// Extraire l'extension
		size_t dotPos = fullPath.find_last_of('.');
		std::string extension;
		if (dotPos != std::string::npos)
			extension = fullPath.substr(dotPos);

		// Vérifier si c'est un script CGI
		if (cgiExtensions.find(extension) != cgiExtensions.end())
		{
			CGI cgi;
			cgi.setMethod("GET");
			cgi.setScriptPath(fullPath);
			cgi.setInterpreter(cgiExtensions.at(extension));

			std::string cgiOutput = cgi.execute();
			response.setStatus(200, "OK");
			response.setBody(cgiOutput);
			response.addHeader("Content-Type", "text/html");
			return;
		}

		// Vérifier si le fichier existe
		struct stat fileStat;
		if (stat(fullPath.c_str(), &fileStat) != 0)
		{
			response.setStatus(404, "Not Found");
			response.setBody("404 Not Found\n");
			response.addHeader("Content-Type", "text/plain");
			return;
		}

		// Si c'est un dossier, chercher index.html
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

		// Lire le fichier
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
		file.close();

		response.setStatus(200, "OK");
		response.setBody(buffer.str());

		// Déterminer le MIME type
		std::string mime = _mimetypes.getMimeType(extension);
		response.addHeader("Content-Type", mime);
	}
	catch (const std::exception &e)
	{
		response.setStatus(500, "Internal Server Error");
		response.setBody("500 Internal Server Error\n");
		response.addHeader("Content-Type", "text/plain");
	}
}

void RequestHandler::handlePost(const HttpRequest &request, Response &response, const Server &server)
{
	try
	{
		const Location &location = selectLocation(server, request.getUri());
		std::string root = location.getRoot();
		std::string uploadPath = location.getUploadPath();
		const std::map<std::string, std::string> &cgiExtensions = location.getCgiExtensions();

		std::string path = request.getUri();
		std::string fullPath = root + path;

		// Extraire l'extension du fichier
		size_t dotPos = fullPath.find_last_of('.');
		std::string extension;
		if (dotPos != std::string::npos)
			extension = fullPath.substr(dotPos);

		const std::string &body = request.getBody();

		// Cas 1 : Script CGI POST
		if (cgiExtensions.find(extension) != cgiExtensions.end())
		{
			CGI cgi;
			cgi.setMethod("POST");
			cgi.setScriptPath(fullPath);
			cgi.setInterpreter(cgiExtensions.at(extension));
			cgi.setBody(body);

			std::string cgiOutput = cgi.execute();
			response.setStatus(200, "OK");
			response.setBody(cgiOutput);
			response.addHeader("Content-Type", "text/html");
			return;
		}

		// Cas 2 : Upload de fichier
		if (body.empty())
		{
			response.setStatus(400, "Bad Request");
			response.setBody("400 POST body is empty\n");
			response.addHeader("Content-Type", "text/plain");
			return;
		}

		// Créer le dossier upload s'il n'existe pas
		if (!uploadPath.empty())
		{
			// Générer un nom de fichier unique (timestamp ou autre)
			std::string filename = "uploaded_file";
			std::string uploadedFilePath = uploadPath + "/" + filename;

			// Écrire le fichier
			std::ofstream outFile(uploadedFilePath.c_str(), std::ios::out | std::ios::binary);
			if (!outFile)
			{
				response.setStatus(403, "Forbidden");
				response.setBody("403 Cannot write file\n");
				response.addHeader("Content-Type", "text/plain");
				return;
			}
			outFile << body;
			outFile.close();

			response.setStatus(201, "Created");
			response.setBody("File uploaded successfully\n");
			response.addHeader("Content-Type", "text/plain");
			return;
		}

		// Cas 3 : Pas de destination d'upload
		response.setStatus(405, "Method Not Allowed");
		response.setBody("405 POST not allowed for this location\n");
		response.addHeader("Content-Type", "text/plain");
	}
	catch (const std::exception &e)
	{
		response.setStatus(500, "Internal Server Error");
		response.setBody("500 Internal Server Error\n");
		response.addHeader("Content-Type", "text/plain");
	}
}

// void RequestHandler::handleDelete(const HttpRequest &request, Response &response, const config &config)
// {

// }

