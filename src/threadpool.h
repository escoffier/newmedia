#pragma once

#include "buffer.h"
#include <mutex>
#include <algorithm>
#include <iostream>
#include <condition_variable>

//template <typename T>
class ThreadPool
{
    struct Thread_wrapper
    {
        Thread_wrapper()
        {
            quit_ = false;
            thread_ = std::thread(&Thread_wrapper::run, this);
        }

        // template <class Function, class... Args>
        // Thread_wrapper(Function &&f, Args &&... args)
        // {
        //     thread_ = std::thread(std::forward<Function>(f), std::forward<Args>(args)...);
        // }

        Thread_wrapper(Thread_wrapper &&t) : thread_(std::move(t.thread_)), buffers_(std::move(t.buffers_))
        {
        }

        ~Thread_wrapper()
        {
            {
                std::lock_guard<std::mutex> lk(bufferMutex_);
                quit_ = true;
            }
            cond_.notify_all();

            if (thread_.joinable())
                thread_.join();
        }

        Thread_wrapper &operator=(const Thread_wrapper &&t) = delete;
        Thread_wrapper(const Thread_wrapper &t) = delete;
        Thread_wrapper &operator=(const Thread_wrapper &t) = delete;

        void stop()
        {
            {
                std::lock_guard<std::mutex> lk(bufferMutex_);
                quit_ = true;
            }
            cond_.notify_all();
        }

        void run()
        {
            while (true)
            {
                std::unique_lock<std::mutex> lk(bufferMutex_);
                //std::cout << "---buffers_ size: " << buffers_.size() << std::endl;
                cond_.wait(lk, [this]() { return (!buffers_.empty()) || quit_; });
                if (quit_)
                {
                    std::cout << "quit thread" << std::endl;
                    return;
                }

                for (auto &b : buffers_)
                {
                    b.second->processData();
                }
            }
        }

        void addBuffer(const std::string &id, std::shared_ptr<Buffer> buffer)
        {
            std::lock_guard<std::mutex> lk(bufferMutex_);
            buffers_.emplace(id, buffer);
            cond_.notify_one();
            std::cout << "addBuffer " << buffers_.size() << std::endl;
        }

        size_t bufferSize()
        {
            std::lock_guard<std::mutex> lk(bufferMutex_);
            return buffers_.size();
        }

        std::shared_ptr<Buffer> findBuffer(const std::string &id)
        {
            std::lock_guard<std::mutex> lk(bufferMutex_);
            auto buf = buffers_.find(id);
            if (buf != buffers_.end())
                return buf->second;
            return std::shared_ptr<Buffer>();
        }

        void removeBuffer(const std::string &id)
        {
            std::lock_guard<std::mutex> lk(bufferMutex_);

            auto buf = buffers_.find(id);
            if (buf != buffers_.end())
            {
                if (buf->second->senderNumber() != 0)
                {
                    buf->second->clearSenders();
                }

                buffers_.erase(buf);
            }
        }

        std::mutex bufferMutex_;
        std::condition_variable cond_;
        std::thread thread_;
        std::map<std::string, std::shared_ptr<Buffer>> buffers_;
        bool quit_;
    };

  public:
    ThreadPool(int n) : threads_(std::vector<Thread_wrapper>(n))
    {
    }

    ~ThreadPool()
    {
        threads_.clear();
    }

    void stop()
    {
        threads_.clear();
    }

    void attachBuffer(std::string id, std::shared_ptr<Buffer> buffer)
    {
        auto th = std::min_element(threads_.begin(), threads_.end(), [](Thread_wrapper &a, Thread_wrapper &b) -> bool {
            return a.bufferSize() < b.bufferSize();
        });

        th->addBuffer(id, buffer);
        //min->buffers_.push_back(buffer);
    }

    std::shared_ptr<Buffer> getBuffer(const std::string &id)
    {
        for (int i = 0; i < threads_.size(); i++)
        {
            auto buf = threads_[i].findBuffer(id);
            if (buf != nullptr)
                return buf;
        }

        return std::shared_ptr<Buffer>();
    }

    void removeBuffer(const std::string &id)
    {
        for (int i = 0; i < threads_.size(); i++)
        {
            auto buf = threads_[i].findBuffer(id);
            if (buf != nullptr)
                threads_[i].removeBuffer(id);
        }
    }

  private:
    void operator()()
    {
    }

    bool shutdown_;
    std::vector<Thread_wrapper> threads_;
    //std::mutex mutex_;
};