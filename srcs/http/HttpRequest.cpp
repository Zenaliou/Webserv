#include "../http/HttpRequest.hpp"

//exemple a quoi ressemble une requete
//arrive par recv() en plusieurs morceaux par flux
//donc faut attendre de tt recevoir
//et le morceau peut meme depasser sur la prochaine requete

//HttpRequest req;

//req.method        // "GET"
//req.uri           // "/index.html"
//req.version       // "HTTP/1.1"
//req.headers["Host"] = "localhost"
//req.body          // "hello=world"

//recv() → buffer
//  ↓
//headers complets ?
//  ↓
//parser headers
//  ↓
//body requis ?
//  ↓
//body complet ?
//  ↓
//COMPLETE

//le header peut etre en minuscule ou en majuscule, ecriture non normaliser
//donc on va tout mettre en minuscule comme ca plus simple pour les recherches

//Request-Line\r\n
//Header-1\r\n
//Header-2\r\n
//...
//Header-N\r\n
//\r\n
//Body (optionnel)

//si ya chunked transfer encoding, le body est divise en chunks
//chaque chunk commence par sa taille en hexadecimale suivi de \r\n
//puis les donnees du chunk suivi de \r\n
//le dernier chunk a une taille de 0
//exemple :
//4\r\n
//Wiki\r\n
//5\r\n
//pedia\r\n
//E\r\n
// in\r\n
//chunks.\r\n
//0\r\n
//\r\n

HttpRequest::HttpRequest() : _expectedBodyLength(0), _errorCode(0) {}

HttpRequest::~HttpRequest(){}

const std::string &HttpRequest::getMethod() const{
	return (_method);
}
const std::string &HttpRequest::getUri() const{
	return (_uri);
}
const std::string &HttpRequest::getHttpVersion() const{
	return (_httpVersion);
}
const std::string &HttpRequest::getBody() const{
	return (_body);
}

const int HttpRequest::getErrorCode() const{
	return (_errorCode);
}

void HttpRequest::setMethod(const std::string &method){
	_method = method;
}

void HttpRequest::setUri(const std::string &uri){
	_uri = uri;
}

void HttpRequest::setBody(const std::string &body){
	_body = body;
}

std::string HttpRequest::getHeaderValue(const std::string& name) const{
	std::map<std::string, std::string>::const_iterator it = _headers.find(name);
	if (it != _headers.end())
		return it->second;
	return "";
}

std::string HttpRequest::toLower(const std::string &str)const{
	std::string lowerStr = str;
	for (size_t i = 0; i < lowerStr.size(); i++)
	{
		lowerStr[i] = std::tolower(static_cast<unsigned char>(str[i]));//cast juste au cas ou c des caracteres accentuees
	}
	return lowerStr;
}

size_t HttpRequest::getContentLength() const{
	std::string contentLength = getHeaderValue("content-length");
	if (contentLength.empty())
		return 0;
	for (size_t i = 0; i < contentLength.size(); i++)
	{
		if (!std::isdigit(contentLength[i]))
			return 0;
	}
	std::istringstream iss(contentLength);
	size_t length = 0;
	iss >> length;
	return length;
}

void HttpRequest::parseRequest(std::string &buffer){
	size_t headerEnd = buffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)//npos = si on a rien trouver avec find et donc le header n'est pas complet
	{
		_errorCode = 400; //Bad Request
		return ;
	}
	std::string headerPart = buffer.substr(0, headerEnd);
	
	//on traite seulement la partie header
	std::istringstream stream(headerPart);
	std::string line;
	
	//chaque ligne du header se termine par "\r\n"
	//et getline va stocker dans line chaque ligne de stream jusqu'a "\n"
	//donc on va supprimer le "\r" qui reste
	if (!line.empty() && line[line.size() -1] == '\r')
		line.erase(line.size() -1);
	//pour la 1ere ligne du header
	if (!parseRequestLine(line))
		return ;
	
	//pour les autres lignes du header
	while (std::getline(stream, line))
	{
		if (!line.empty() && line[line.size() -1] == '\r')
			line.erase(line.size() -1);
		if (!line.empty())
			parseHeader(line);
	}

	//la partie body
	size_t bodyStart = headerEnd + 4; // + 4 pour "\r\n\r\n"
	size_t totalSize = bodyStart;

	if (hasHeader("content-length"))
	{
		_expectedBodyLength = getContentLength();
		if (buffer.size() - bodyStart < _expectedBodyLength)
		{
			_errorCode = 400;
			return ;
		}
		_body = buffer.substr(bodyStart, _expectedBodyLength);
		totalSize += _expectedBodyLength;
		//if ((bodyStart + _expectedBodyLength) <= buffer.size())
		//{
		//	_body = buffer.substr(bodyStart, _expectedBodyLength);
		//}
	}
	else if (isChunked())
	{
		if (!parseChunkBody(buffer, bodyStart))
			return ;
	}
}

bool HttpRequest::parseRequestLine(const std::string &line){
	std::istringstream stream(line);
	if (!(stream >> _method >> _uri >> _httpVersion))
	{
		_errorCode = 400; //400 = bad request
		return false;
	}
	
	//verifie la methode
	for (size_t i = 0; i < _method.size(); i++)
		_method[i] = std::toupper(_method[i]);
	if (_method != "GET" && _method != "POST" && _method != "DELETE")//! check avec les methodes autorisees pour differencier 405 (Not allowed) et 501 (Not implemented)
	{
		_errorCode = 405; //405 = method not allowed
		return false;
	}

	//verifie l'uri
	if (_uri.empty() || _uri[0] != '/')
	{
		_errorCode = 400;
		return false;
	}

	//verifie la version http
	if (_httpVersion != "HTTP/1.1") //je sais pas s'il faut rajouter HTTP/1.0
	{
		_errorCode = 505; // 505 = http version not supported
		return false;
	}
	return true;
}

void HttpRequest::parseHeader(const std::string &line){
	size_t pos = line.find(':');
	if (pos == std::string::npos)
	{
		return;
	}
	std::string name = line.substr(0, pos);
	std::string value = line.substr(pos + 1); // on saute le ":", pas normer donc peut avoir ensuite plusieurs espaces ou rien
	size_t startValue = value.find_first_not_of(" \t");
	if (startValue != std::string::npos)
	{
		value = value.substr(startValue);
	}
	size_t endValue = value.find_last_not_of(" \n\t\r");
	if (endValue != std::string::npos)
	{
		value = value.substr(0, endValue + 1);
		//+1 car find_last_not_of renvoie l'index et substr a besoin du nbre de caractere
		//donc si index 4, nbre de caractere 5
	}

	//stock en minuscule pour faciliter la recherche
	_headers[toLower(name)] = value;
}

bool HttpRequest::hasHeader(const std::string &name) const{
	if (_headers.find(toLower(name)) != _headers.end())
		return true;
	else
		return false;
}

bool HttpRequest::isChunked() const{
	std::string encoding = getHeaderValue("transfer-encoding");
	if (encoding.find("chunked") != std::string::npos)
		return true;
	return false;
}

bool HttpRequest::parseChunkBody(const std::string &data, size_t bodyStart){
// ! Recheck des return false(), il faut bien differencier une requete incomplete (attente) et une requete erronee (Bad request)
// ? Peut-etre utiliser une variable _chunkBodyComplete
	size_t pos = bodyStart;
	_body.clear();

	while (pos < data.size())
	{
		//lire la taille du chunk en hexadecimale
		size_t lineEnd = data.find("\r\n", pos);
		if (lineEnd == std::string::npos)
			// return false;	//pas assez de donnees pour lire la taille du chunk
							// ? Requete coupee ou Requete invalide (400)
			continue ;
		std::string chunkSizeLine = data.substr(pos, lineEnd - pos);

		//si apres la taille hexa, il y a une extension apres ;
		//on s'en fout de l'extension, on garde que la taille
		size_t semicolon = chunkSizeLine.find(';');
		if (semicolon != std::string::npos)
		{
			chunkSizeLine = chunkSizeLine.substr(0, semicolon);
		}

		size_t chunkSize = 0;
		std::istringstream iss(chunkSizeLine);
		iss >> std::hex >> chunkSize;//converti hexa en decimal

		if (iss.fail())
		{
			_errorCode = 400;
			return false; //erreur de conversion
		}

		pos = lineEnd + 2;//saute "\r\n"

		if (chunkSize == 0)//dernier chunk, fin du body
		{
			//si pas de trailers donc apres avoir sauter les "\r\n", il y a direct encore "\r\n"
			if (pos + 2 <= data.size() && data.substr(pos, 2) == "\r\n")
				return true;
			//il y a des trailers cad dans le body il y a une partie des headers
			//donc on doit avoir "\r\n\r\n"
			size_t finalEnd = data.find("\r\n\r\n", pos);
			if (finalEnd != std::string::npos)
				return true;
			return false;//chunk incomplet
		}

		if (pos + chunkSize + 2 > data.size())
		{
			// return false;	//pas assez de donnees pour lire le chunk complet
			// 				//! NE pas renvoyer un code d'erreur
			continue ;
		}

		_body.append(data, pos, chunkSize);
		if (data.substr(pos + chunkSize, 2) != "\r\n")
		{
			_errorCode = 400;
			return false;
		}
		else
			pos += chunkSize + 2; //saute le chunk et le "\r\n"
	}
	_errorCode = 400;
	return false;
}
