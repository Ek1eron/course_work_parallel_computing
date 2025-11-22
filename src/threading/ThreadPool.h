#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class ThreadPool {
public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();

    template<class F>
    auto enqueue(F&& f) -> std::future<decltype(f())>;

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_ = false;
};

template<class F>
auto ThreadPool::enqueue(F&& f) -> std::future<decltype(f())> {
    using RetType = decltype(f());

    auto taskPtr = std::make_shared<std::packaged_task<RetType()>>(std::forward<F>(f));
    std::future<RetType> res = taskPtr->get_future();

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        if (stop_) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks_.emplace([taskPtr]() { (*taskPtr)(); });
    }

    condition_.notify_one();
    return res;
}
