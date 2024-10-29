// TaskScheduler.cpp

#include "TaskScheduler.h"

TaskScheduler::TaskScheduler(size_t thread_count) : running_(true) {
    for (size_t i = 0; i < thread_count; ++i) {
        workers_.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(task_mutex_);
                    task_cv_.wait(lock, [this] {
                        return !tasks_.empty() || !running_;
                    });
                    if (!running_ && tasks_.empty()) return;
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                try {
                    task();
                }
                catch (const std::exception& e) {
                    std::cerr << "Error executing task: " << e.what() << std::endl;
                }
            }
        });
    }
}

void TaskScheduler::Add(std::function<void()> task, std::time_t timestamp) {
    std::lock_guard<std::mutex> lock(schedule_mutex_);
    scheduled_tasks_.emplace(timestamp, std::move(task));
    schedule_cv_.notify_one();
}

void TaskScheduler::Start() {
    scheduler_ = std::thread([this]() {
        while (running_) {
            std::unique_lock<std::mutex> lock(schedule_mutex_);
            if (scheduled_tasks_.empty()) {
                schedule_cv_.wait(lock);
            }
            else {
                auto now = std::time(nullptr);
                auto next_task_time = scheduled_tasks_.top().first;
                if (next_task_time <= now) {
                    auto task = scheduled_tasks_.top().second;
                    scheduled_tasks_.pop();
                    lock.unlock();

                    {
                        std::lock_guard<std::mutex> task_lock(task_mutex_);
                        tasks_.push(task);
                    }
                    task_cv_.notify_one();
                }
                else {
                    schedule_cv_.wait_until(lock, std::chrono::system_clock::from_time_t(next_task_time));
                }
            }
        }
    });
}

void TaskScheduler::Stop() {
    if (!running_) return;
    running_ = false;
    schedule_cv_.notify_all();
    task_cv_.notify_all();

    if (scheduler_.joinable()) scheduler_.join();

    for (auto& worker : workers_) {
        if (worker.joinable()) worker.join();
    }
}

TaskScheduler::~TaskScheduler() {
    Stop();
}
