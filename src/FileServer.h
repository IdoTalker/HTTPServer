#pragma once
#include <string>
#include <fstream>
#include "HttpResponse.h"

HttpResponse serveFile(const std::string &path)
{
    std::string filePath = "../public" + path;
    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open())
    {
        HttpResponse res;
        res.statusCode = 404;
        res.statusText = "Not Found";
        res.body = "<h1>404 Not Found</h1>";
        res.headers["Content-Type"] = "text/html";
        return res;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::string contentType = "text/plain";
    size_t dot = path.rfind('.');
    if (dot != std::string::npos)
    {
        std::string ext = path.substr(dot);
        if (ext == ".html")
            contentType = "text/html";
        else if (ext == ".css")
            contentType = "text/css";
        else if (ext == ".js")
            contentType = "application/javascript";
        else if (ext == ".png")
            contentType = "image/png";
        else if (ext == ".jpg")
            contentType = "image/jpeg";
    }

    HttpResponse res;
    res.statusCode = 200;
    res.statusText = "OK";
    res.body = content;
    res.headers["Content-Type"] = contentType;
    res.headers["Content-Length"] = std::to_string(content.size());
    return res;
}