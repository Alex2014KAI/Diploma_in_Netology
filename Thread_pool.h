#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include <condition_variable>
#include <functional>
#include <vector>

#include "globals.h"
#include "Spider.h"

using namespace std;


namespace SPIDER
{
    template <typename T>
    class safe_queue {
    public:
        void push(T fn) {
            lock_guard<mutex> lg(mt); // We grab a mutex so that no one else can add a function to the thread.
            queue_.push(std::move(fn));
            cv.notify_all();
        }

        T pop() {
            unique_lock<std::mutex> ul(mt);
            cv.wait(ul, [this]() { return !queue_.empty(); });
            T func = std::move(queue_.front());
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
        Thread_pool(): spiderSetup("ini.txt") {
            numberThreads = _Thrd_hardware_concurrency();
            maxRecursionLevel = spiderSetup.depthRecursion_;
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
            while (true) {
                Link link = queueURL.pop();
                cout << "Сайт " << link.url_ << " анализируется потоком:" << this_thread::get_id() << endl;
                
                Spider spider(spiderSetup.dataSetupBD_, maxRecursionLevel);
                spider.execute(link);

                std::vector<Link> currentLinkPage = spider.getLinksOnTheCurrentSiteSpider_Link();
                link_Table.insert(link_Table.end(), currentLinkPage.begin(), currentLinkPage.end());      

                while (!link_Table.empty()) {
                    Link link = link_Table.front();
                    link_Table.erase(link_Table.begin());
                    submit(link);
                }

            }
        };

        void submit(Link link) {
            if (link.currentRecursionLevel_ == maxRecursionLevel) return;
            std::cout << "Добавлен сайт:  " << link.url_ << std::endl;
            queueURL.push(link);
        };


    private:
        int numberThreads;
        vector<thread> vectorThread;
        safe_queue<Link> queueURL;

        SpiderSetup spiderSetup;
        std::vector<Link> link_Table;
        mutex mt1;
        int maxRecursionLevel;
    };
}
