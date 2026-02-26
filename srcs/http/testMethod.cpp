//#include <iostream>
//#include <fstream>
//#include <sys/stat.h>

//#include "RequestHandler.hpp"
//#include "Response.hpp"

//// Petit helper pour créer un dossier si besoin
//void createDirectory(const std::string &path)
//{
//    mkdir(path.c_str(), 0755);
//}

//int main()
//{
//    try
//    {
//        // ===============================
//        // 1️⃣ Création config manuelle
//        // ===============================
//        config conf;

//        Server server;
//        server.host = "127.0.0.1";
//        server.port = 8080;
//        server.serverName = "testServer";
//        server.root = "./www";
//        server.index = "index.html";
//        server.autoindex = false;
//        server.allowedMethods.push_back("GET");
//        server.allowedMethods.push_back("POST");
//        server.allowedMethods.push_back("DELETE");

//        Location loc;
//        loc.path = "/";
//        loc.root = "./www";
//        loc.index = "index.html";
//        loc.autoindex = false;
//        loc.uploadPath = "./www/uploads";
//        loc.clientMaxBodySize = 1000000;

//        loc.allowedMethods.push_back("GET");
//        loc.allowedMethods.push_back("POST");

//        // Exemple CGI (.py)
//        loc.cgiExtensions[".py"] = "/usr/bin/python3";

//        server.locations.push_back(loc);

//        // ⚠️ Comme config n'a pas de addServer(),
//        // il faut modifier directement le vector interne
//        const_cast<std::vector<Server>&>(conf.getServers()).push_back(server);

//        // ===============================
//        // 2️⃣ Préparer environnement test
//        // ===============================
//        createDirectory("./www");
//        createDirectory("./www/uploads");

//        // Créer un index.html
//        std::ofstream indexFile("./www/index.html");
//        indexFile << "<html><body><h1>Hello Webserv</h1></body></html>";
//        indexFile.close();

//        RequestHandler handler;

//        // ===============================
//        // 3️⃣ TEST GET
//        // ===============================
//        HttpRequest getReq;
//        Response getRes;

//        getReq.setMethod("GET");
//        getReq.setUri("/");
//        getReq.setBody("");  // GET = pas de body

//        handler.handleRequest(getReq, getRes, conf, 8080);

//        std::cout << "========= GET RESPONSE =========\n";
//        std::cout << getRes.generateResponse() << std::endl;

//        // ===============================
//        // 4️⃣ TEST POST (upload)
//        // ===============================
//        HttpRequest postReq;
//        Response postRes;

//        postReq.setMethod("POST");
//        postReq.setUri("/upload");
//        postReq.setBody("Test upload content");

//        handler.handleRequest(postReq, postRes, conf, 8080);

//        std::cout << "========= POST RESPONSE =========\n";
//        std::cout << postRes.generateResponse() << std::endl;
//    }
//    catch (const std::exception &e)
//    {
//        std::cerr << "Erreur: " << e.what() << std::endl;
//    }

//    return 0;
//}

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

#include "method.hpp"
#include "Response.hpp"
#include "HttpRequest.hpp"

// Helper : créer dossier si inexistant
void createDirectory(const std::string &path)
{
    mkdir(path.c_str(), 0755);
}

// Helper : vérifier si fichier existe
bool fileExists(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

int main()
{
    try
    {
        // ============================
        // 1️⃣ Préparer environnement
        // ============================
        createDirectory("./www");

        std::string testFilePath = "./www/test.txt";

        // Créer un fichier à supprimer
        std::ofstream file(testFilePath.c_str());
        file << "File to be deleted";
        file.close();

        if (fileExists(testFilePath))
            std::cout << "Fichier créé avec succès.\n";

        // ============================
        // 2️⃣ Configurer Location
        // ============================
        Location location;
        location.path = "/";          // URL de base
        location.root = "./www";      // Dossier réel

        // ============================
        // 3️⃣ Créer requête DELETE
        // ============================
        HttpRequest request;
        request.setMethod("DELETE");
        request.setUri("/test.txt");   // On veut supprimer ce fichier
        request.setBody("");

        Response response;

        // ============================
        // 4️⃣ Appel handleDelete
        // ============================
        method m;
        m.handleDelete(request, response, location);

        // ============================
        // 5️⃣ Afficher réponse HTTP
        // ============================
        std::cout << "\n===== RESPONSE =====\n";
        std::cout << response.generateResponse() << std::endl;

        // ============================
        // 6️⃣ Vérifier suppression
        // ============================
        if (!fileExists(testFilePath))
            std::cout << "✅ Fichier supprimé avec succès.\n";
        else
            std::cout << "❌ Le fichier existe encore.\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erreur: " << e.what() << std::endl;
    }

    return 0;
}