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


#include <csignal>
#include <cstdio>
#include <cstdlib>
using namespace std;


namespace SPIDER
{
    
    template <typename T>
    class safe_queue {
    public:
        void push(T fn) {
            {
                lock_guard<mutex> lg(mt); // We grab a mutex so that no one else can add a function to the thread.
                queue_.push(std::move(fn));
                std::cout << "                                 Длинна внутреннего вектора " << queue_.size() << std::endl;
            }
            //cv.notify_all();
            cv.notify_one();
        }

        T pop() {
            unique_lock<std::mutex> ul(mt);
            cv.wait(ul, [this]() { return !queue_.empty(); });
            T func = std::move(queue_.front());
            queue_.pop();
            ul.unlock();
            return func;
        }

        bool empty() {
            std::lock_guard<std::mutex> lock(mt);
            return queue_.empty();
        };
    private:
        mutex mt;
        condition_variable cv;
        std::queue<T> queue_;
    };

    class Thread_pool {
    public:
        Thread_pool(): spiderSetup("ini.txt") {
            numberThreads = std::thread::hardware_concurrency() - 5;
            //numberThreads = 2;
            std::cout << "Число потоков: " << numberThreads << std::endl;
            std::cout << "*********" << active_threads.load() << "********" << std::endl;
            maxRecursionLevel = spiderSetup.depthRecursion_;
            for (int i{ 0 }; i < (numberThreads); i++) {
                // vectorThread.push_back(thread(&Thread_pool::work, this));
                vectorThread.emplace_back(thread(&Thread_pool::work, this));
            }
        };

        ~Thread_pool() {
            for (int i{ 0 }; i < (numberThreads); i++) {
                vectorThread[i].join();
            }
        };

        void work() {
            int a = 0;
            while (true) {
                try {
                {
                    std::lock_guard<std::mutex> lock(mutex_iteration);
                    active_threads.fetch_add(1, std::memory_order_relaxed);
                }
                
                Link link = queueURL.pop();
                cout << "Сайт " << link.url_ << " анализируется потоком:" << this_thread::get_id() << endl;

                
                    Spider spider(spiderSetup.dataSetupBD_, maxRecursionLevel);
                    spider.execute(link);
                
                {
                    std::lock_guard<std::mutex> lock(mutex_iteration);

                    std::vector<Link> currentLinkPage = spider.getLinksOnTheCurrentSiteSpider_Link();
                    link_Table.insert(link_Table.end(), currentLinkPage.begin(), currentLinkPage.end());

                    active_threads.fetch_sub(1, std::memory_order_relaxed);
                }
                
#ifndef DEBUG_TRHEDPULL               
                while (!link_Table.empty()) { // в условии должен быть флаг о том, что какой то из потоков свободен .... Или вынести while в main что бы не зависал поток
                    std::lock_guard<std::mutex> lock(mutex_iteration);
                    if (active_threads.load() <= (numberThreads)) {
                        std::cout << "*********" << active_threads.load() << "********" << std::endl;
                        for (int i{ 0 }; i < active_threads.load(); i++) {
                            if (!link_Table.empty()) {
                                Link link = link_Table.front();
                                link_Table.erase(link_Table.begin());
                                std::cout << "                                     число сайтов в векторе ссылок " << link_Table.size() << std::endl;
                                submit(link);
                                
                            };
                        }
                        //break;
                    }

                }
#endif // !DEBUG_TRHEDPULL
                
                }
                catch (std::exception& e) {
                    std::cerr << "Исключение: " << e.what() << std::endl;
                }

            }
            
        };

       

        void workDebug() {
            int a = 0;
            while (true) {

                Link link = queueURL.pop();
                // cout << "Сайт " << link.url_ << " анализируется потоком:" << this_thread::get_id() << endl;
                std::cout << a << std::endl;
                a++;
                Spider spider(spiderSetup.dataSetupBD_, maxRecursionLevel);

                spider.execute(link);

                std::vector<Link> currentLinkPage = spider.getLinksOnTheCurrentSiteSpider_Link();

                link_Table.insert(link_Table.end(), currentLinkPage.begin(), currentLinkPage.end());

                // Нужен флаг который бы следил за тем что сайт просмотрен
            }
        };
        
        std::atomic<int> active_threads{ 0 }; // счетчик активных потоков
        std::condition_variable cv_iteration_end; // условная переменная
        std::mutex mutex_iteration; // мьютекс для cv



        // Решение проблемы с потоком

        void submit(Link link) {
            if (link.currentRecursionLevel_ > maxRecursionLevel) return;
            // std::cout << "Добавлен сайт:  " << link.url_ << std::endl;
            queueURL.push(link);
        };

        std::vector<Link> link_Table;
        int numberThreads;
    private:
        
        vector<thread> vectorThread;
        safe_queue<Link> queueURL;

        SpiderSetup spiderSetup;
        
        mutex mt1;
        int maxRecursionLevel;
    };
    

}
