#pragma once
#include <functional>
#include <mutex>
#include <queue>
#include <vector>

class ThreadPool {
public:
    ThreadPool();
    ~ThreadPool();

    void queueJob(const std::function<void()> &job);
    void done();
    bool isBusy();
    void join();    /* TODO: wait all job finish */

private:
    void threadLoop();

    bool should_terminate = false;           // Tells threads to stop looking for jobs
    std::mutex queue_mutex;                  // Prevents data races to the job queue
    std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
};