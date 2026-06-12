#pragma once
#include <winsock2.h>

struct SocketGuard {
    SOCKET s;

    SocketGuard(SOCKET s) : s(s) {}
    ~SocketGuard() { if (s != INVALID_SOCKET) closesocket(s); }

    SocketGuard(const SocketGuard&) = delete;
    SocketGuard& operator=(const SocketGuard&) = delete;
};