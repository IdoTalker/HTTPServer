#include "Socket.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "FileServer.h"
#include <iostream>

int main()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    SocketGuard serverSocket(socket(AF_INET, SOCK_STREAM, 0));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    bind(serverSocket.s, (sockaddr *)&addr, sizeof(addr));
    listen(serverSocket.s, SOMAXCONN);

    std::cout << "listening..." << std::endl;

    while (true)
    {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SocketGuard clientSocket(accept(serverSocket.s, (sockaddr *)&clientAddr, &clientAddrLen));

        char buffer[4096];
        int bytesRead = recv(clientSocket.s, buffer, sizeof(buffer), 0);

        HttpRequest req = parseRequest(std::string(buffer, bytesRead));
        std::cout << req.method << " " << req.path << std::endl;

        if (req.path == "/")
            req.path = "/index.html";

        HttpResponse res = serveFile(req.path);
        std::string responseStr = buildResponse(res);
        send(clientSocket.s, responseStr.c_str(), responseStr.size(), 0);
    }

    WSACleanup();
}