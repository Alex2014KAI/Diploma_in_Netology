#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include <condition_variable>
#include <functional>
#include <vector>

#include "globals.h"

using namespace std;


namespace SPIDER
{
    template <typename T>
    class safe_queue {
    public:
        void push(T fn) {
            lock_guard<mutex> lg(mt); // We grab a mutex so that no one else can add a function to the thread.
            queue_.push(std::move(fn));
            cv.notify_one();
        }

        T pop() {
            unique_lock<std::mutex> ul(mt);
            cv.wait(ul, [this]() { return !queue_.empty(); });
            auto func = std::move(queue_.front());
            queue_.pop();
            return func;
        }
    private:
        mutex mt;
        condition_variable cv;
        std::queue<T> queue_;
    };

    class Thread_pool {
    public:
        Thread_pool() {
            numberThreads = _Thrd_hardware_concurrency();
            //std::cout << numberThreads << std::endl;
            for (int i{ 0 }; i < (numberThreads - 3); i++) {
                vectorThread.push_back(thread(&Thread_pool::work, this));
            }
        };

        ~Thread_pool() {
            for (int i{ 0 }; i < (numberThreads - 3); i++) {
                vectorThread[i].join();
            }
        };

        void work() {
            std::function<void()> currentTask;

            while (true) {
                currentTask = queueFunction.pop();
                cout << "Сайт анализируется потоком:" << this_thread::get_id() << endl;
                currentTask();

            }
        };

        void submit(std::function<void()> fn, std::string url) {
            std::cout << "Добавлен сайт:  " << url << std::endl;
            queueFunction.push(move(fn));
        };


    private:
        int numberThreads;
        vector<thread> vectorThread;
        safe_queue<std::function<void()>> queueFunction;
    };

}
