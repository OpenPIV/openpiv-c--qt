
#pragma once

// from https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h
//
// licence: zlib licence
// https://github.com/progschj/ThreadPool/blob/master/COPYING
//
// Copyright (c) 2012 Jakob Progsch, Vaclav Zeman
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//    1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
//    2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
//    3. This notice may not be removed or altered from any source
//    distribution.
//
// Modified to remove std::future usage i.e. no returns, and to propagate
// exceptions

// std
#include <exception>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool
{
public:
    // the constructor just launches some amount of workers
    ThreadPool(size_t threads)
    {
        auto worker_func =
            [this]()
            {
                while(true)
                {
                    try
                    {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);

                            // wait and check if we actually have work to do upon wakeup
                            this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });

                            // if we've been signalled to stop, only do so once the queue is empty
                            if(this->stop && this->tasks.empty())
                                return;

                            // take a task and pop
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        task();
                    }
                    catch (std::exception& e)
                    {
                        std::cerr << e.what() << "\n";
                        std::abort();
                        // ... or save the exception and check later
                    }
                }
            };

        // add threads
        for (size_t i = 0;i<threads;++i)
            workers.emplace_back(worker_func);
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for(std::thread &worker: workers)
            worker.join();
    }

    template <typename F>
    auto enqueue(F&& task) -> std::enable_if_t<std::is_invocable_v<decltype(task)>>
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace(task);
        }

        condition.notify_one();
    }

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::queue< std::function<void()> > tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;
};
