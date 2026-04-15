#include "../inc/RequestHandler.hpp"

RequestHandler::RequestHandler() {}

RequestHandler::~RequestHandler() {}

std::string RequestHandler::getTimeStamp() const
{
	time_t now = time(0);
	struct tm datetime;
	char buf[50];

	datetime = *gmtime(&now);
	// Format: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
	strftime(buf, sizeof(buf), "%Y_%m_%d_%H%M%S", &datetime);
	return std::string(buf);
}

void RequestHandler::handleRequest(const RequestParser &request, Response &response)
{
	try
	{
		if (request.getErrorCode() != 0)
		{
			int	errorCode = request.getErrorCode();
			handleRequestError(response, errorCode);
			return ;
		}
		const Location &location = request.getLocation();
		const std::string &method = request.getMethod();
		if (method == "GET")
			handleGet(request, response, location);
		else if (method == "POST")
			handlePost(request, response, location);
		else if (method == "DELETE")
			handleDelete(request, response, location);
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

void RequestHandler::handleRequestError(Response &response, int errorCode)
{
	switch (errorCode)
	{
		case 405:
			setErrorResponseTemp(response, errorCode, "Method Not Allowed", "405 Method Not Allowed\n");
			break;
		case 413:
			setErrorResponseTemp(response, errorCode, "Payload Too Large", "413 Payload Too Large\n");
			break;
		case 505:
			setErrorResponseTemp(response, errorCode, "HTTP Version Not Supported", "505 HTTP Version Not Supported\n");
			break;
		default:
			setErrorResponseTemp(response, errorCode, "Bad Request", "400 Bad Request\n");
			break;
	}
}

void RequestHandler::handleGet(const RequestParser &request, Response &response, const Location &location)
{
	std::string root = location.getRoot();
	const std::map<std::string, std::string> &cgiExtensions = location.getCgiExtensions();

	std::string path = request.getUri();
	if (path == "/")
		path.append(location.getIndex());

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
		setErrorResponse(response, 404, "Not Found", "www/html/Error404.html");
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

static bool extractFileFromMultipart(const std::string &body, const std::string &boundary, std::string &filename, std::string &filecontent)
{
	size_t pos = body.find("Content-Disposition:");
	if (pos == std::string::npos)
		return (false);

	size_t filenamePos = body.find("filename=\"", pos);
	if (filenamePos == std::string::npos)
		return (false);
	filenamePos += 10;
	size_t filenameEnd = body.find("\"", filenamePos);
	if (filenameEnd == std::string::npos)
		return (false);

	filename = body.substr(filenamePos, filenameEnd - filenamePos);
	if (filename.empty())
		return (false);

	size_t contentStart = body.find("\r\n\r\n", pos);
	if (contentStart == std::string::npos)
		return (false);
	contentStart += 4;

	std::string boundaryEnd = "\r\n--" + boundary;
	size_t contentEnd = body.find(boundaryEnd, contentStart);
	if (contentEnd == std::string::npos)
		return (false);

	filecontent = body.substr(contentStart, contentEnd - contentStart);
	return (true);
}

static std::string getFileExtension(const std::string &filename)
{
	size_t dotPos = filename.find_last_of('.');
	if (dotPos != std::string::npos && dotPos < filename.length() -1)
		return (filename.substr(dotPos));
	return ("");
}

void RequestHandler::handlePost(const RequestParser &request, Response &response, const Location &location)
{
	std::string root = location.getRoot();
	std::string uploadPath = location.getUploadPath();
	const std::map<std::string, std::string> &cgiExtensions = location.getCgiExtensions();

	std::string path = request.getUri();
	std::string fullPath = root + path;

	// Extraire l'extension du fichier
	size_t dotPos = fullPath.find_last_of('.');
	std::string extension;
	if (dotPos != std::string::npos && dotPos != 0)
		extension = fullPath.substr(dotPos);
	else
		extension = "";

	const std::string &body = request.getBody();

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

	if (body.empty())
	{
		response.setStatus(400, "Bad Request");
		response.setBody("400 POST body is empty\n");
		response.addHeader("Content-Type", "text/plain");
		return;
	}

	if (!uploadPath.empty())
	{
		std::string originalFilename;
		std::string fileContent = body;

		std::string contentType = request.getHeaderValue("content-type");
		size_t boundaryPos = contentType.find("boundary=");

		if (boundaryPos != std::string::npos)
		{
			std::string boundary = contentType.substr(boundaryPos + 9);
			if (!boundary.empty() && boundary[0] == '"')
				boundary = boundary.substr(1, boundary.length() - 2);
			if (!extractFileFromMultipart(body, boundary, originalFilename, fileContent))
				originalFilename = "";
		}

		std::string filename = getTimeStamp();
		std::string uploadedFilePath = uploadPath + "/" + filename;
		if (!originalFilename.empty())
		{
			std::string ext = getFileExtension(originalFilename);
			if (!ext.empty())
				uploadedFilePath += ext;
		}

		std::cout << uploadedFilePath << std::endl;

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
	response.setStatus(405, "Method Not Allowed");
	response.setBody("405 POST not allowed for this location\n");
	response.addHeader("Content-Type", "text/plain");
}

void RequestHandler::handleDelete(const RequestParser &request, Response &response, const Location& location){
	std::string path = request.getUri();//uri demander par le client dans sa requete
	if (path.find(location.path) == 0)//location.path = chemin defini dans la config
		path = path.substr(location.path.size());//si l'uri commence par le chemin defini dans la config, on enleve cette partie
	std::string fullPath = location.getUploadPath();
	fullPath += path;

	struct stat statBuf;//structure qui va contenir les infos d'un fichier
	//par ex : le type(fichier ou dossier), les permissions, la taille du fichier, la date de modif...

	//stat() = fonction qui remplit la structure
	if (stat(fullPath.c_str(), &statBuf) != 0)//le fichier n'existe pas ou on n'a pas acces ou erreur systeme
	{
		setErrorResponse(response, 404, "Not Found", "www/html/Error404.html");
		return;
	}

	//si c'est un repertoire et donc pas un fichier, on ne peut pas supprimer
	if (S_ISDIR(statBuf.st_mode))
	{
		response.setStatus(403, "Forbidden");
		response.setBody("Error 403: Forbidden, can not delete directories\n");
		response.addHeader("Content-Type", "text/plain");
		return;
	}

	//unlink() = supprime un fichier
	if (unlink(fullPath.c_str()) == 0)
	{
		response.setStatus(204, "No Content");
		response.setBody("");//contenu vide
	}

	//on doit choisir soit code 204 no content soit code 200 avec un body
	//A FAIRE AJOUTER content_length et content-type
	//if (unlink(fullPath.c_str()) == 0)
	//{
	//	response.setStatus(200, "OK");
	//	response.setBody("File deleted\n");
	//	response.addHeader("Content-Type", "text/plain");
	//}
	else
	{
		response.setStatus(500, "Internal server error");
		response.setBody("Error 500: Could not delete file\n");
		response.addHeader("Content-Type", "text/plain");
	}
}

void RequestHandler::setErrorResponse(Response &response, int code, const std::string &reason_phrase, const std::string &file)
{
	response.setStatus(code, reason_phrase);
	response.setBody(loadFile(file));
	response.addHeader("Content-Type", "text/html");
}

void RequestHandler::setErrorResponseTemp(Response &response, int code, const std::string &reason_phrase, const std::string &body)
{
	response.setStatus(code, reason_phrase);
	response.setBody(body + "\n");
	response.addHeader("Content-Type", "text/plain");
}

std::string loadFile(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
		return "<h1>Erreur: fichier introuvable</h1>";

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}
