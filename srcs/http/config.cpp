#include "config.hpp"

config::config(){}

config::config(const std::string &filename){
	parseConfigFile(filename);

	if (_servers.empty())
		throw std::runtime_error("no configuration");
}

config::~config(){}

std::string config::trim(const std::string &str){
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";
	
	size_t end = str.find_last_not_of(" \t\r\n");
	return (str.substr(start, end - start + 1));
}

std::vector<std::string> config::split(const std::string &str, char delim){
	std::vector<std::string> result;
	std::istringstream stream(str);
	std::string token;

	while (std::getline(stream, token, delim))
	{
		token = trim(token);
		if (!token.empty())
			result.push_back(token);
	}
	return result;
}

void config::parseConfigFile(const std::string &filename){
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		throw std::runtime_error("Can not open config file");
	}

	std::string line;
	while (std::getline(file, line))
	{
		line = cleanLine(line);
		if (line.empty())
			continue;
		
		if (line.find("server") == 0)
		{
			bool foundOpenBracket = false;
			if (line.find('{') != std::string::npos)
			{
				foundOpenBracket = true;
				line.erase(line.find('{'), 1);
				line = trim(line);
			}
			std::vector<std::string> tokens;
			std::istringstream iss(line);//car separe direct en enlevant les espaces, tab...
			std::string word;
			while (iss >> word)
			{
				tokens.push_back(word);
			}
			if (tokens.size() == 1 && tokens[0] == "server")
			{
				Server server;
				parseServer(file, server, foundOpenBracket);
				_servers.push_back(server);
			}
			else
			{
				throw std::runtime_error("Invalid config : '{' missing");
			}
		}
		else
		{
			throw std::runtime_error("Invalid config : server missing");
		}
	}
	file.close();
}

void config::parseServer(std::ifstream &file, Server& server, bool bracket){
	std::string line;
	bool foundOpenBracket = bracket;

	if (!foundOpenBracket)
	{
		while (std::getline(file, line))
		{
			line = cleanLine(line);
			if (line.empty())
				continue;
			if (line == "{")
			{
				foundOpenBracket = true;
				break;
			}
			else
				throw std::runtime_error("Expected '{' after server");
		}
	}

	while (std::getline(file, line))
	{
		line = cleanLine(line);
		if (line.empty())
			continue;
		if (line == "}")
			break;
		if (!line.empty() && line[line.size() - 1] == ';')
		{
			line = line.substr(0, line.size() - 1);
			line = trim(line);
		}

		std::vector<std::string> tokens;
		std::istringstream iss(line);//car separe direct en enlevant les espaces, tab...
		std::string word;
		while (iss >> word)
		{
			tokens.push_back(word);
		}
		//std::vector<std::string>tokens = split(line, ' ');
		if (tokens.empty())
			continue;
		
		std::string directive = tokens[0];
		if (directive == "listen" && tokens.size() >= 2)
		{
			try{
				int port = stringToInt(tokens[1]);
				if (port < 1024 || port > 65535)//car en dessous port resever pour root et au dessus existe pas
					throw std::out_of_range("port");
				server.port = port;
			}
			catch (const std::exception&)
			{
				std::cerr << "Port invalide\n";
			}
		}

		else if (directive == "server_name" && tokens.size() >= 2)
			server.serverName = tokens[1];
		else if (directive == "error_page" && tokens.size() >= 3)
		{
			try{
				int code = stringToInt(tokens[1]);
				if (code < 400 || code > 599)
					throw std::out_of_range("Invalid error code");
				server.errorPages[code] = tokens[2];
			}
			catch (const std::exception&)
			{
				std::cerr << "error_page : code invalide \n";
			}
		}
		else if (directive == "client_max_body_size" && tokens.size() >= 2)
		{
			try {
				int size = stringToInt(tokens[1]);
				if (size <= 0)
					throw std::out_of_range("Invalid body size");
				server.clientMaxBodySize = size;
			}
			catch (const std::exception&){
				std::cerr << "client_max_body_size_invalide\n";
			}
		}
		else if (directive == "root" && tokens.size() >= 2)
			server.root = tokens[1];
		else if (directive == "index" && tokens.size() >= 2)
			server.index = tokens[1];
		else if (directive == "autoindex" && tokens.size() >= 2)
		{
			if (tokens[1] == "on")
				server.autoindex = true;
			else if (tokens[1] == "off")
				server.autoindex = false;
			else
				throw std::runtime_error("autoindex must be on or off");
				//car tokens.size() >= 2 verifie juste qu'il y a une valeur
				//mais si valeur fausse, autre que on ou off, il faut error
		}
		else if (directive == "allowed_methods" && tokens.size() >= 2)
		{
			for (size_t i = 1; i < tokens.size(); i++)
			{
				server.allowedMethods.push_back(tokens[i]);
			}
		}
		else if (directive == "location" && tokens.size() >= 2)
		{
			bool foundOpenBracket = false;
			if (line.find('{') != std::string::npos)
			{
				foundOpenBracket = true;
				line.erase(line.find('{'), 1);
				line = trim(line);
			}
			Location location;
			location.path = tokens[1];
			location.root = server.root;
			location.index = server.index;
			location.autoindex = server.autoindex;
			location.clientMaxBodySize = server.clientMaxBodySize;
			location.allowedMethods = server.allowedMethods;
			parseLocation(file, location, foundOpenBracket);
			server.locations.push_back(location);
		}
		else
			throw std::runtime_error("unknown directive");
			//pas juste std::cerr car sinon arrete pas lep programme
			//avec throw quitte tt et signale une erreur
	}
}

void config::parseLocation(std::ifstream &file, Location &location, bool bracket){
	std::string line;
	bool foundOpenBracket = bracket;

	if (!foundOpenBracket)
	{
		while (std::getline(file, line))
		{
			line = cleanLine(line);
			if (line.empty())
				continue;
			if (line == "{")
			{
				foundOpenBracket = true;
				break;
			}
			else
				throw std::runtime_error("Expected '{'");
		}
	}
	//while (std::getline(file, line))
	//{
	//	line = trim(line);
	//	if (line == "{")
	//		break;
	//}

	while (std::getline(file, line))
	{
		line = cleanLine(line);
		if (line.empty())
			continue;
		if (line == "}")
			break;
		if (!line.empty() && line[line.size() - 1] == ';')
		{
			line = line.substr(0, line.size() - 1);
			line = trim(line);
		}
		std::vector<std::string> tokens = split(line, ' ');
		if (tokens.empty())
			continue;
		
		std::string directive = tokens[0];
		if (directive == "autoindex" && tokens.size() >= 2)
		{
			if (tokens[1] == "on")
				location.autoindex = true;
			else if (tokens[1] == "off")
				location.autoindex = false;
			else
				throw std::runtime_error("autoindex must be on or off");
				//car tokens.size() >= 2 verifie juste qu'il y a une valeur
				//mais si valeur fausse, autre que on ou off, il faut error
		}
		else if (directive == "allowed_methods" && tokens.size() >= 2)
		{
			for (size_t i = 1; i < tokens.size(); i++)
			{
				location.allowedMethods.push_back(tokens[i]);
			}
		}
		else if (directive == "index" && tokens.size() >= 2)
			location.index = tokens[1];
		else if (directive == "client_max_body_size" && tokens.size() >= 2)
		{
			try {
			int size = stringToInt(tokens[1]);
			if (size <= 0)
				throw std::out_of_range("Invalid body size");
			location.clientMaxBodySize = size;
			}
			catch (const std::exception&){
				std::cerr << "client_max_body_size_invalide\n";
			}
		}
		else if (directive == "cgi_extension" && tokens.size() >= 3)
			location.cgiExtensions[tokens[1]] = tokens[2];
		else if (directive == "upload_path" && tokens.size() >= 2)
			location.uploadPath = tokens[1];
		else if (directive == "root" && tokens.size() >= 2)
			location.root = tokens[1];
		else
			throw std::runtime_error ("unknown directive");
	}
}

std::string config::cleanLine(const std::string &line){
	std::string cleaned = trim(line);

	size_t pos = cleaned.find('#');
	if (pos != std::string::npos)
		cleaned = cleaned.substr(0, pos);
	return trim(cleaned);
}


const std::vector<Server>&config::getServers() const{
	return _servers;
}

int stringToInt(const std::string &str){
	std::istringstream iss(str);
	int value;
	iss >> value;

	if (iss.fail() || !iss.eof())
		throw std::runtime_error("Invalid integer");
	return value;
}