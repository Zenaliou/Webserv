#ifndef CONFIG_HPP
# define CONFIG_HPP

#define FAKE_UNLIMITED_SIZE 104857600

#include <string>
#include <vector>
#include <map>
#include <fstream>//pour std::getline
#include <sstream>//pour istringstream
#include <iostream>//cerr
struct Location {
	std::string path;
	bool autoindex;
	std::string index;
	std::vector<std::string> allowedMethods;//conteneur dynamique, alloc et liberation auto
	size_t clientMaxBodySize;
	std::map<std::string, std::string> cgiExtensions;
	std::string uploadPath;
	std::string root;

	const std::string &getPath() const{return path;}
	bool getAutoindex() const{return autoindex;}
	const std::string &getIndex() const{return index;}
	const std::vector<std::string> &getAllowedMethods() const{return allowedMethods;}
	size_t getClientMaxBodySize() const{return clientMaxBodySize;}
	const std::map<std::string, std::string> &getCgiExtensions() const{return cgiExtensions;}
	const std::string &getUploadPath() const{return uploadPath;}
	const std::string &getRoot() const{return root;}
};
struct Server {
	std::string host;
	int port;
	std::vector<std::string> serverNames;
	size_t clientMaxBodySize;
	std::map<int, std::string> errorPages;
	std::string root;
	std::string index;
	bool autoindex;
	std::vector<std::string> allowedMethods;
	std::vector<Location> locations;

	Server() : host("0.0.0.0"), port(8080), clientMaxBodySize(FAKE_UNLIMITED_SIZE), autoindex(false){}
	//on initialise ces 4 valeurs car le reste vide par defaut

	const std::string &getHost() const{return host;}
	const std::vector<std::string> &getServerNames() const{return serverNames;}
	int getPort() const{return port;}
};
class config{
	private:
	std::vector<Server>_servers;//car on peut avoir plusieurs serveurs

	public:
	config();
	config(const std::string &filename);
	//config(const config& other);
	~config();

	//config& operator= (const config& other);

	void parseConfigFile(const std::string &filename);
	void parseServer(std::ifstream &file, Server& server, bool bracket);
	void parseLocation(std::ifstream &file, Location &location, bool bracket);
	void parseListen(const std::string &value, Server &server);
	const std::vector<Server>&getServers() const;
};

int stringToInt(const std::string &str);
std::string trim(const std::string &str);
std::vector<std::string> split(const std::string &str, char delim);
std::string cleanLine(const std::string &line);

#endif
