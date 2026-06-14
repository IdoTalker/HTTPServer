#pragma once
#include <string>
#include <map>
#include <sstream>
#include "UrlUtils.h"

struct HttpRequest
{
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    std::map<std::string, std::string> queryParams;
    std::map<std::string, std::string> params;
};

HttpRequest parseRequest(const std::string &raw)
{
    HttpRequest req;

    size_t index = raw.find("\r\n");
    std::string requestline = raw.substr(0, index);
    std::istringstream ss(requestline);
    std::string a, b, c;
    ss >> a >> b >> c;

    req.method = a;

    size_t splitIndex = b.find('?');
    if (splitIndex != std::string::npos)
    {
        req.path = urlDecode(b.substr(0, splitIndex));
        req.queryParams = parseQueryString(b.substr(splitIndex + 1));
    }
    else
    {
        req.path = urlDecode(b);
    }

    req.version = c;

    size_t cursor = index + 2;

    while (true)
    {
        size_t lineEnd = raw.find("\r\n", cursor);
        std::string line = raw.substr(cursor, lineEnd - cursor);

        if (line.empty())
            break;

        size_t colon = line.find(": ");

        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 2);

        req.headers[key] = value;

        cursor = lineEnd + 2;
    }

    req.body = raw.substr(cursor);

    return req;
}