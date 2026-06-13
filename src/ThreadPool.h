#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>

struct ThreadPool
{
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;

    ThreadPool(int numThreads);
    ~ThreadPool();
    void enqueue(std::function<void()> task);
};

ThreadPool::ThreadPool(int numThreads)
{
    for (int i = 0; i < numThreads; i++)
    {
        workers.emplace_back([this]()
                             {
            while(true){
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this]() {
                        return !tasks.empty() || stop;
                    });
                    if(stop && tasks.empty()) return;
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