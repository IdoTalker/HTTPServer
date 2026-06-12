#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <WinSock2.h>
#include "ClientHandler.h"

struct ThreadPool
{
    std::vector<std::thread> workers;
    std::queue<SOCKET> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;

    ThreadPool(int numThreads);
    ~ThreadPool();
    void enqueue(SOCKET clientSocket);
};

ThreadPool::ThreadPool(int numThreads)
{
    for (int i = 0; i < numThreads; i++)
    {
        workers.emplace_back([this]()
                             {
            while(true){
                SOCKET clientSocket;
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this]() {
                        return !tasks.empty() || stop;
                    });
                    if(stop && tasks.empty()) return;
                    clientSocket = tasks.front();
                    tasks.pop();
                }
                handleClient(clientSocket);
            } });
    }
}

void ThreadPool::enqueue(SOCKET clientSocket)
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        tasks.push(clientSocket);
    }
    cv.notify_one();
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();
    for (auto &t : workers)
        t.join();
}