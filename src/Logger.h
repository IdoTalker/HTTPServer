#pragma once
#include <ctime>
#include <iostream>
#include <string>

void log(const std::string &method, const std::string &path, int statusCode)
{
    time_t now = time(nullptr);
    char timeBuf[32];
    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    std::cout << "[" << timeBuf << "] " << method << " " << path << " " << statusCode << std::endl;
}