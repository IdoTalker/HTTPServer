#include "ClientHandler.h"
#include <iostream>
#include <thread>

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

        SOCKET raw = clientSocket.s;
        clientSocket.s = INVALID_SOCKET; // So SocketGuard wont close our socket when the loop closes

        std::thread t(handleClient, raw);
        t.detach(); // We don't care when the function finished so we use detach()
    }

    WSACleanup();
}