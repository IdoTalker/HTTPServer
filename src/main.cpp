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

    sockaddr_in addr;
    addr.sin_family = AF_INET; //means we are using IPV4
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    bind(serverSocket.s, (sockaddr *)&addr, sizeof(addr));
    listen(serverSocket.s, SOMAXCONN);

    std::cout << "listening..." << std::endl;

    ThreadPool pool(4);

    while (true)
    {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SocketGuard clientSocket(accept(serverSocket.s, (sockaddr *)&clientAddr, &clientAddrLen));

        SOCKET raw = clientSocket.s;
        pool.enqueue(raw);
        clientSocket.s = INVALID_SOCKET; // So SocketGuard wont close our socket when the loop closes
    }

    WSACleanup();
}