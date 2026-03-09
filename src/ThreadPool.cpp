#include "ThreadPool.h"
#include "TaskQueue.h"
#include <thread>
#include <vector>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>

namespace SmartCachePP {

class ThreadPool {
public:
    ThreadPool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty())
                            return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.push(std::move(task));
        }
        condition.notify_one();
    }

    void shutdown() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (auto& worker : workers) {
            if (worker.joinable())
                worker.join();
        }
    }

    ~ThreadPool() {
        shutdown();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

}