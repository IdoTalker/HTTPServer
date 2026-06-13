#pragma once
#include <string>

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