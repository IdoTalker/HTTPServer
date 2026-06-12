#pragma once
#include <string>
#include <map>

struct HttpResponse
{
    int statusCode;
    std::string statusText;
    std::map<std::string, std::string> headers;
    std::string body;
};

std::string buildResponse(const HttpResponse &res)
{
    std::string response = "HTTP/1.1 " + std::to_string(res.statusCode) + " " + res.statusText + "\r\n";

    for (const auto &header : res.headers)
    {
        response += header.first + ": " + header.second + "\r\n";
    }

    response += "\r\n";
    response += res.body;

    return response;
}