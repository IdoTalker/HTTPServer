#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <format>

void log(const std::string &method, const std::string &path, int statusCode)
{
    auto now = std::chrono::system_clock::now();
    auto secs = std::chrono::floor<std::chrono::seconds>(now);

    std::cout << std::format("[{:%Y-%m-%d %H:%M:%S}] {} {} {}\n", secs, method, path, statusCode);
}