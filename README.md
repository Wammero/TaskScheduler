# TaskScheduler

A multithreaded task scheduler implemented in C++. This scheduler allows for the scheduling and execution of tasks at specified future timestamps using a thread pool. It employs a priority queue to manage scheduled tasks, ensuring that tasks are executed in the correct order based on their scheduled time.

## Features

- **Multithreading**: Utilizes a pool of worker threads to execute tasks concurrently.
- **Task Scheduling**: Schedule tasks to run at specific times using timestamps.
- **Error Handling**: Catches exceptions during task execution and reports errors to the console.
- **Graceful Shutdown**: Provides a method to stop the scheduler cleanly, ensuring all tasks complete before exiting.

## Requirements

- C++11 or higher
- A standard-compliant C++ compiler

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/Wammero/TaskScheduler.git
   cd TaskScheduler
2. Compile the project:
   ```bash
   g++ main.cpp TaskScheduler.cpp -o TaskScheduler -lpthread
## Usage
- Initialize the TaskScheduler with the desired number of worker threads.
- Add tasks using the Add method, providing a callable task and a timestamp for when the task should execute.
- Start the scheduler with the Start method.
- Stop the scheduler with the Stop method when you are done.


## Example
Here's an example of how to use the TaskScheduler:
```C++
#include <iostream>
#include <thread>
#include <chrono>
#include "TaskScheduler.h"

int main() {
    TaskScheduler scheduler(4);
    scheduler.Start();

    scheduler.Add([] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Task 1 executed\n";
    }, std::time(nullptr) + 1);

    scheduler.Add([] {
        std::this_thread::sleep_for(std::chrono::seconds(0));
        std::cout << "Task 2 executed\n";
    }, std::time(nullptr) + 2);

    while(true) {};

    scheduler.Stop();
    return 0;
}
```