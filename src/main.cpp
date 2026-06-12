#include "ClientHandler.h"
#include "ThreadPool.h"
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
    if (serverSocket.s == INVALID_SOCKET)
    {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET; // means we are using IPV4
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(serverSocket.s, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    if (listen(serverSocket.s, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    std::cout << "listening..." << std::endl;

    ThreadPool pool(4);

    while (true)
    {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SocketGuard clientSocket(accept(serverSocket.s, (sockaddr *)&clientAddr, &clientAddrLen));

        SOCKET raw = clientSocket.s;
        if (raw == INVALID_SOCKET)
        {
            std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        pool.enqueue(raw);
        clientSocket.s = INVALID_SOCKET; // So SocketGuard wont close our socket when the loop closes
    }

    WSACleanup();
}