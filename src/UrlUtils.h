#pragma once
#include <string>
#include <map>
#include <algorithm>

std::string urlDecode(const std::string &path)
{
    std::string result;
    for (size_t i = 0; i < path.size(); i++)
    {
        if (path[i] == '%')
        {
            result += (char)std::stoul(path.substr(i + 1, 2), nullptr, 16);
            i += 2;
        }
        else
        {
            result += path[i];
        }
    }

    return result;
}

std::map<std::string, std::string> parseQueryString(const std::string &query)
{
    std::map<std::string, std::string> map;
    if (query.empty())
        return map;
    std::string currQuery = query;

    while (currQuery.find('&') != std::string::npos)
    {
        size_t equalsIndex = currQuery.find('=');
        std::string currKey = currQuery.substr(0, equalsIndex);
        size_t nextIndex = currQuery.find('&');
        if (nextIndex == std::string::npos)
        {
            nextIndex = currQuery.size();
        }
        std::string currValue = currQuery.substr(equalsIndex + 1, nextIndex - equalsIndex - 1);

        std::replace(currKey.begin(), currKey.end(), '+', ' ');
        std::replace(currValue.begin(), currValue.end(), '+', ' ');

        map[urlDecode(currKey)] = urlDecode(currValue);

        currQuery = currQuery.substr(nextIndex + 1);
    }

    size_t equalsIndex = currQuery.find('=');
    std::string lastKey = currQuery.substr(0, equalsIndex);
    std::string lastValue = currQuery.substr(equalsIndex + 1, currQuery.size());

    std::replace(lastKey.begin(), lastKey.end(), '+', ' ');
    std::replace(lastValue.begin(), lastValue.end(), '+', ' ');

    map[urlDecode(lastKey)] = urlDecode(lastValue);

    return map;
}