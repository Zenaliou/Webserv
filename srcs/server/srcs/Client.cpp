#include "../inc/Client.hpp"
#include "../inc/Librairy.hpp"

/*
handleClient() - Handle data from a connected client
This function is called when poll() detects data available on client socket.
Returns:
  1  = keep connection alive (more data may come)
  0  = client disconnected gracefully (recv returned 0)
  -1 = error occurred
HTTP request format:
  GET / HTTP/1.1\r\n
  Host: localhost\r\n
  \r\n              <- empty line signals end of headers
We accumulate data in buffer_in until we see "\r\n\r\n" (end of headers)
*/
int Client::handleClient()
{
	char buffer[4096];
	
	// recv() reads data from socket into buffer
	// Returns: bytes read, 0 if disconnected, -1 if error
	ssize_t bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read > 0)
	{
		buffer[bytes_read] = '\0';  // null-terminate for string operations
		buffer_in += buffer;         // accumulate data
		
		// Verifier si on a au moins les headers complets
		size_t headerEnd = buffer_in.find("\r\n\r\n");
		if (headerEnd == std::string::npos)
			return (1); // headers incomplets, attendre plus de donnees
		
		// Extraire Content-Length du header si present
		size_t contentLengthPos = buffer_in.find("Content-Length:");
		if (contentLengthPos == std::string::npos)
			contentLengthPos = buffer_in.find("content-length:");
		
		if (contentLengthPos != std::string::npos && contentLengthPos < headerEnd)
		{
			// Trouver la valeur de Content-Length
			size_t valueStart = buffer_in.find(":", contentLengthPos) + 1;
			size_t valueEnd = buffer_in.find("\r\n", valueStart);
			std::string contentLengthStr = buffer_in.substr(valueStart, valueEnd - valueStart);
			
			// Supprimer les espaces
			size_t firstNonSpace = contentLengthStr.find_first_not_of(" \t");
			if (firstNonSpace != std::string::npos)
				contentLengthStr = contentLengthStr.substr(firstNonSpace);
			
			// Convertir en nombre
			size_t contentLength = 0;
			std::istringstream iss(contentLengthStr);
			iss >> contentLength;
			
			// Verifier si on a tout le body
			size_t bodyStart = headerEnd + 4;
			size_t bodyReceived = buffer_in.size() - bodyStart;
			
			if (bodyReceived < contentLength)
				return (1); // body incomplet, attendre plus de donnees
		}
		
		// On a tout recu, parser et repondre
		request.parseRequest(buffer_in, *_config, _port);
		Response reponse;
		RequestHandler handler;
		handler.handleRequest(request, reponse);

		// Send Response
		buffer_out = reponse.generateResponse();
		send(fd, buffer_out.c_str(), buffer_out.size(), 0);

		buffer_in.clear();
		request = RequestParser(); // reset
		// else
		// {
		// 	Response response;
		// 	response.setStatus(400, "Bad Request\n");
		// 	response.setBody("Error 400: Bad request");
		// 	buffer_out = response.generateResponse();
		// 	send(fd, buffer_out.c_str(), buffer_out.size(), 0);
			
		// 	buffer_in.clear();
		// 	request = RequestParser();
		// 	return (0);
		// }
		return (1); // keep connection alive
	}

	if (bytes_read == 0) // recv() returns 0 when client closes connection
	{
		closeConnection();
		return (0);
	}

	// bytes_read < 0 means error
	// EAGAIN/EWOULDBLOCK = no data available right now (normal in non-blocking)
	if (errno != EAGAIN && errno != EWOULDBLOCK)
	{
		closeConnection();
		return (-1);
	}
	return (1); // no data available, try again later
}

/*
closeConnection() - Close the client socket
Closes the file descriptor and marks it as invalid (-1)
to prevent double-close.
*/
void Client::closeConnection()
{
	if (fd >= 0)
	{
		close(fd);
		fd = -1;
	}
}
