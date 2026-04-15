#include <iostream>
#include "../srcs/http/RequestParser.hpp"

int main() {
    // Exemple de requête HTTP complète
    std::string rawRequest =
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 12\r\n"
        "Connection: keep-alive\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "\r\n"
        "hello=world!";

    RequestParser request;

    bool complete = request.parseRequest(rawRequest);
    if (!complete) {
        std::cout << "Requête incomplète\n";
        return 1;
    }

    std::cout << "=== Requête parsée ===\n";
    std::cout << "Méthode : " << request.getMethod() << "\n";
    std::cout << "URI : " << request.getUri() << "\n";
    std::cout << "Version HTTP : " << request.getHttpVersion() << "\n";
    std::cout << "Body : " << request.getBody() << "\n";

    std::cout << "Headers :\n";
    std::cout << "Host : " << request.getHeaderValue("host") << "\n";
    std::cout << "Content-Length : " << request.getHeaderValue("content-length") << "\n";
    std::cout << "Content-Type : " << request.getHeaderValue("content-type") << "\n";

    return 0;
}
