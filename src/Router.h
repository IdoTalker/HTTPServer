#pragma once
#include <map>
#include <string>
#include <functional>
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "FileServer.h"

struct Router
{
    std::map<std::pair<std::string, std::string>, std::function<HttpResponse(const HttpRequest &)>> routes;

    void addRoute(const std::string &method, const std::string &path, std::function<HttpResponse(const HttpRequest &)> handler)
    {
        routes[{method, path}] = handler;
    }

    HttpResponse route(const HttpRequest &req)
    {
        auto it = routes.find({req.method, req.path});
        HttpResponse res;

        if (it != routes.end())
        {
            res = it->second(req);
        }
        else
        {
            res = serveFile(req.path);
        }

        return res;
    }
};