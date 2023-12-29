/*
Implementation of thread pool.
Taken from https://stackoverflow.com/questions/15752659/thread-pooling-in-c11
*/

#include <vector>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "ThreadPool.h"

/*Start the pool of threads*/
void ThreadPool::Start()
{
    int maxThreads = std::thread::hardware_concurrency(); // One thread for every
    for (int i = 0; i < maxThreads; i++)
    {
        threads.emplace_back(std::thread(ThreadPool::ThreadLoop));
    }
}

void ThreadPool::ThreadLoop()
{
    std::function<void()> job;
    while (true)
    {
        std::unique_lock<std::mutex> queue_lock(queue_mutex);
        mutex_condition.wait(queue_lock, [this]
                             { return !jobs.empty() || should_terminate; });
        if (should_terminate)
        {
            return;
        }

        job = jobs.front();
        jobs.pop();
        job();
    }
}

void ThreadPool::QueueJob(const std::function<void()> &job)
{
    std::unique_lock<std::mutex> queue_lock(queue_mutex);
    jobs.push(job);
    mutex_condition.notify_one();
}

bool ThreadPool::busy()
{
    std::unique_lock<std::mutex> queue_lock(queue_mutex);
    return !jobs.empty();
}

void ThreadPool::Stop()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    should_terminate = true;
    mutex_condition.notify_all();
    for (std::thread &active_thread : threads)
    {
        active_thread.join();
    }
    threads.clear();
}
