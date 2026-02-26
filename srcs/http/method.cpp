#include "method.hpp"

method::method(){}

method::~method(){}


void method::handleDelete(const HttpRequest &request, Response &response, const Location& location){
	std::string path = request.getUri();//uri demander par le client dans sa requete
	if (path.find(location.path) == 0)//location.path = chemin defini dans la config
		path = path.substr(location.path.size());//si l'uri commence par le chemin defini dans la config, on enleve cette partie
	
	std::string fullPath = location.root;
	if (fullPath[fullPath.size() - 1] != '/')
		fullPath += '/';
	fullPath += path;

	struct stat statBuf;//structure qui va contenir les infos d'un fichier
	//par ex : le type(fichier ou dossier), les permissions, la taille du fichier, la date de modif...

	//stat() = fonction qui remplit la structure
	if (stat(fullPath.c_str(), &statBuf) != 0)//le fichier n'existe pas ou on n'a pas acces ou erreur systeme
	{
		response.setStatus(404, "Not Found");
		response.setBody("Error 404: File not found\n");
		response.addHeader("Content-Type", "text/plain");
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
