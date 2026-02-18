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
	std::string root;
	std::map<std::string, std::string> cgiExtensions;
	for (std::vector<Location>::const_iterator location_it = server.locations.begin(); location_it != server.locations.end(); ++location_it)
	{
		root = location_it->getRoot();
		cgiExtensions = location_it->getCgiExtensions();
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

	if (cgiExtensions.find(extension) != cgiExtensions.end()) // CGI
	{
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

void RequestHandler::handlePost(const HttpRequest &request, Response &response, const Server &server)
{
	std::string root;
	std::string uploadPath;
	std::map<std::string, std::string> cgiExtensions;

	for (std::vector<Location>::const_iterator location_it = server.locations.begin(); location_it != server.locations.end(); ++location_it)
	{
		root = location_it->getRoot();
		uploadPath = location_it->getUploadPath();
		cgiExtensions = location_it->getCgiExtensions();
	}

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
		cgi.setInterpreter(cgiExtensions[extension]);
		cgi.setBody(body);
		// Ajouter les headers si nécessaire
		// cgi.setHeaders(request.getHeaders());

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

// void RequestHandler::handleDelete(const HttpRequest &request, Response &response, const config &config)
// {

// }

