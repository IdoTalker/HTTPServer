#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>

struct ThreadPool
{
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable_any cv;
    std::vector<std::jthread> workers;

    ThreadPool(int numThreads);
    void enqueue(std::function<void()> task);
};

ThreadPool::ThreadPool(int numThreads)
{
    for (int i = 0; i < numThreads; i++)
    {
        workers.emplace_back([this](std::stop_token st)
                             {
            while(true){
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    if(!cv.wait(lock, st, [this]() {
                        return !tasks.empty();
                    })) return;
                    task = tasks.front();
                    tasks.pop();
                }
                task();
            } });
    }
}

void ThreadPool::enqueue(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        tasks.push(task);
    }
    cv.notify_one();
}