#pragma once
#include "Socket.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "FileServer.h"
#include "Logger.h"
#include <iostream>

void handleClient(SOCKET clientSocket)
{
    while (true)
    {
        char buffer[4096];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if(bytesRead <= 0) break;

        HttpRequest req = parseRequest(std::string(buffer, bytesRead));

        if (req.path == "/")
            req.path = "/index.html";

        HttpResponse res = serveFile(req.path);
        std::string responseStr = buildResponse(res);

        log(req.method, req.path, res.statusCode);

        send(clientSocket, responseStr.c_str(), responseStr.size(), 0);

        std::string connection = req.headers.count("Connection") ? req.headers["Connection"] : "close";
        if(connection == "close") break;
    }

    closesocket(clientSocket);
}