#pragma once
#include "Socket.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Logger.h"
#include "Router.h"
#include "UrlUtils.h"
#include <iostream>

std::string readRequest(SOCKET clientSocket)
{
    std::string data;
    char buffer[4096];

    size_t headerEnd = data.find("\r\n\r\n");
    size_t contentLength = 0;

    // in this loop we read until we find the end of the header
    while (headerEnd == std::string::npos)
    {
        int bytesRead(recv(clientSocket, buffer, sizeof(buffer), 0));

        if (bytesRead <= 0)
            return "";

        data += std::string(buffer, bytesRead);
        headerEnd = data.find("\r\n\r\n");
    }

    size_t bodyStart = headerEnd + 4;
    size_t contentLengthIndex(data.find("Content-Length: "));

    if (contentLengthIndex != std::string::npos)
    {
        size_t clEnd = data.find("\r\n", contentLengthIndex);
        contentLength = std::stoul(data.substr(contentLengthIndex + 16, clEnd - contentLengthIndex - 16));
    }

    //in this loop we read the rest of the message
    while (data.size() < bodyStart + contentLength)
    {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0)
            return "";
        data += std::string(buffer, bytesRead);
    }

    return data;
}

void handleClient(SOCKET clientSocket, Router& router)
{
    while (true)
    {
        std::string raw = readRequest(clientSocket);
        if (raw.empty())
            break;
        HttpRequest req = parseRequest(raw);
        req.path = urlDecode(req.path);

        if (req.path == "/")
            req.path = "/index.html";

        HttpResponse res = router.route(req);
        std::string responseStr = buildResponse(res);

        log(req.method, req.path, res.statusCode);

        send(clientSocket, responseStr.c_str(), responseStr.size(), 0);

        std::string connection = req.headers.count("Connection") ? req.headers["Connection"] : "close";
        if (connection == "close")
            break;
    }

    closesocket(clientSocket);
}