// TaskScheduler.h

#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <iostream>
#include <queue>
#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ctime>
#include <vector>

class TaskScheduler {
public:
    TaskScheduler(size_t thread_count);
    void Add(std::function<void()> task, std::time_t timestamp);
    void Start();
    void Stop();
    ~TaskScheduler();

private:
    using ScheduledTask = std::pair<std::time_t, std::function<void()>>;
    struct CompareTasks {
        bool operator()(const ScheduledTask& lhs, const ScheduledTask& rhs) const {
            return lhs.first > rhs.first;
        }
    };

    std::priority_queue<ScheduledTask, std::vector<ScheduledTask>, CompareTasks> scheduled_tasks_;
    std::mutex schedule_mutex_;
    std::condition_variable schedule_cv_;

    std::queue<std::function<void()>> tasks_;
    std::mutex task_mutex_;
    std::condition_variable task_cv_;

    std::vector<std::thread> workers_;
    std::thread scheduler_;
    bool running_;
};

#endif // TASK_SCHEDULER_H
