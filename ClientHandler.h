#pragma once
#include "Socket.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "FileServer.h"
#include <iostream>

void handleClient(SOCKET clientSocket)
{
    char buffer[4096];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    HttpRequest req = parseRequest(std::string(buffer, bytesRead));
    std::cout << req.method << " " << req.path << std::endl;

    if (req.path == "/")
        req.path = "/index.html";

    HttpResponse res = serveFile(req.path);
    std::string responseStr = buildResponse(res);
    
    send(clientSocket, responseStr.c_str(), responseStr.size(), 0);

    closesocket(clientSocket);
}