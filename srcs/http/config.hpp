#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>//pour std::getline
#include <sstream>//pour istringstream
#include <iostream>//cerr

struct Server {
	std::string host;
	int port;
	std::string serverName;
	size_t clientMaxBodySize;
	std::map<int, std::string> errorPages;
	std::string root;
	std::string index;
	std::vector<Location> locations;

	Server() : host("0.0.0.0"), port(8080), clientMaxBodySize(1000000){}
	//on initialise ces 3 valeurs car le reste vide par defaut
};

struct Location {
	std::string path;
	bool autoindex;
	std::string index;
	std::vector<std::string> allowedMethods;//conteneur dynamique, alloc et liberation auto
	size_t clientMaxBodySize;
	std::string cgiExtension;
	std::string cgiPath;
};

class config{
	private:

	public:
	config();
	config(const config& other);
	~config();

	config& operator= (const config& other);

	void parseConfigFile(const std::string &filename);
	void parseServer(std::ifstream &file, Server& server);
	void parseLocation();
	std::string trim(const std::string &str);
	std::vector<std::string> split(const std::string &str, char delim);
};

#endif