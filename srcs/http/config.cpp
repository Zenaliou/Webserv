#include "config.hpp"

config::config(const std::string &filename){
	parseConfigFile(filename);

	if ()
}

config::~config(){}

std::string config::trim(const std::string &str){

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

void config::parseServer(std::ifstream &file, Server& server){
	std::string line;

	while (std::getline(file, line))
	{
		line = trim(line);
		if (line == "{")
			break;
	}

	while (std::getline(file, line))
	{
		line = trim(line);

		if (line.empty() || line[0] == '#')
			continue;
		if (line == "}")
			break;
		if (!line.empty() && line[line.size() - 1] == ';')
		{
			line = line.substr(0, line.size() - 1);
			line = trim(line);
		}

		std::vector<std::string>tokens = split(line, ' ');
		if (tokens.empty())
			continue;
		
		std::string directive = tokens[0];
		if (directive == "listen" && tokens.size() >= 2)
		{
			try{
				int port = std::stoi(tokens[1]);
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
				int code = std::stoi(tokens[1]);
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
				int size = std::stoi(tokens[1]);
				if (size <= 0)
					throw std::out_of_range("Invalid body size");
				server.clientMaxBodySize = size;
			}
			catch (const std::exception&){
				std::cerr << "client_max_body_size_invalide\n";
			}
		}
		else if (directive == "location" && tokens.size() >= 2)
		{
			Location location;
			location.path = tokens[1];
			location.autoindex = false;//si on a rien, false par defaut
			location.clientMaxBodySize = server.clientMaxBodySize;
			parseLocation(file, location);
			server.locations.push_back(location);
		}
	}
}