#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <stdexcept>

class ThreadPool
{
public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();

    template<class F>
    void enqueue(F&& f);

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_ = false;
};

template<class F>
void ThreadPool::enqueue(F&& f)
{
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        if (stop_)
        {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        tasks_.emplace(std::forward<F>(f));
    }

    condition_.notify_one();
}
