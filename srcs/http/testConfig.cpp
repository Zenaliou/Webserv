#include <iostream>
#include "config.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./webserv <config_file>\n";
        return 1;
    }

    try
    {
        config conf(argv[1]);

        std::cout << "Config parsed successfully!\n";

        const std::vector<Server>& servers = conf.getServers();

        for (size_t i = 0; i < servers.size(); ++i)
        {
            const Server& s = servers[i];

            std::cout << "\n==============================\n";
            std::cout << "        SERVER " << i + 1 << "\n";
            std::cout << "==============================\n";

            std::cout << "Port: " << s.port << "\n";
            std::cout << "Server name: " << s.serverName << "\n";
            std::cout << "Root: " << s.root << "\n";
            std::cout << "Index: " << s.index << "\n";
            std::cout << "Autoindex: " << (s.autoindex ? "on" : "off") << "\n";
            std::cout << "Client max body size: " << s.clientMaxBodySize << "\n";

            // Error pages
            std::cout << "\nError pages:\n";
            for (std::map<int, std::string>::const_iterator it = s.errorPages.begin();
                 it != s.errorPages.end(); ++it)
            {
                std::cout << "  " << it->first << " -> " << it->second << "\n";
            }

            // Allowed methods
            std::cout << "\nAllowed methods:\n";
            for (size_t j = 0; j < s.allowedMethods.size(); ++j)
                std::cout << "  " << s.allowedMethods[j] << "\n";

            // Locations
            std::cout << "\nLocations:\n";
            for (size_t j = 0; j < s.locations.size(); ++j)
            {
                const Location& loc = s.locations[j];

                std::cout << "\n  -------- Location " << j + 1 << " --------\n";
                std::cout << "  Path: " << loc.path << "\n";
                std::cout << "  Root: " << loc.root << "\n";
                std::cout << "  Index: " << loc.index << "\n";
                std::cout << "  Autoindex: " << (loc.autoindex ? "on" : "off") << "\n";
                std::cout << "  Client max body size: " << loc.clientMaxBodySize << "\n";
                std::cout << "  Upload path: " << loc.uploadPath << "\n";

                std::cout << "  Allowed methods:\n";
                for (size_t k = 0; k < loc.allowedMethods.size(); ++k)
                    std::cout << "    " << loc.allowedMethods[k] << "\n";

                std::cout << "  CGI extensions:\n";
                for (std::map<std::string, std::string>::const_iterator it = loc.cgiExtensions.begin();
                     it != loc.cgiExtensions.end(); ++it)
                {
                    std::cout << "    " << it->first << " -> " << it->second << "\n";
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}