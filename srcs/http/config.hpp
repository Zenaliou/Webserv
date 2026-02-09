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
	bool autoindex;
	std::vector<std::string> allowedMethods;
	std::vector<Location> locations;

	Server() : host("0.0.0.0"), port(8080), clientMaxBodySize(1000000), autoindex(false){}
	//on initialise ces 4 valeurs car le reste vide par defaut
};

struct Location {
	std::string path;
	bool autoindex;
	std::string index;
	std::vector<std::string> allowedMethods;//conteneur dynamique, alloc et liberation auto
	size_t clientMaxBodySize;
	std::map<std::string, std::string> cgiExtensions;
	std::string uploadPath;
	std::string root;
};

class config{
	private:
	std::vector<Server>_servers;//car on peut avoir plusieurs serveurs

	public:
	config(const std::string &filename);
	//config(const config& other);
	~config();

	//config& operator= (const config& other);

	void parseConfigFile(const std::string &filename);
	void parseServer(std::ifstream &file, Server& server);
	void parseLocation(std::ifstream &file, Location &location);
	std::string trim(const std::string &str);
	std::vector<std::string> split(const std::string &str, char delim);

	const std::vector<Server>&getServers() const;
};

#endif