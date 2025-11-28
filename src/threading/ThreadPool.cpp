#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threads)
{
    workers_.reserve(threads);

    for (size_t i = 0; i < threads; ++i)
    {
        workers_.emplace_back([this]()
        {
            while (true)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    condition_.wait(lock, [this]()
                    {
                        return stop_ || !tasks_.empty();
                    });

                    if (stop_ && tasks_.empty())
                        return;

                    task = std::move(tasks_.front());
                    tasks_.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        stop_ = true;
    }

    condition_.notify_all();

    for (auto& w : workers_)
    {
        if (w.joinable())
            w.join();
    }
}
