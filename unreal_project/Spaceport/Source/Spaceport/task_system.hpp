#pragma once

#include "concurrent_queue.hpp"


struct task_system
{
    task_system()
    {
        threads_.reserve(n_);
        for (unsigned int j = 0; j < n_; ++j) {
            threads_.emplace_back([this, j] { run(j); });
        }
    }

    ~task_system()
    {
        for (auto & queue : queues_) {
            queue.done();
        }
        for (auto & thread : threads_) {
            thread.join();
        }
    }

    template <typename F>
    void async_exec(F && f) {
        auto const i = index_++;
        for (unsigned j = 0; j < n_; ++j) {
            if (queues_[(i + j) % n_].try_push((F &&)f))
                return;
        }
        queues_[i % n_].push((F &&)f);
    }

private:
    void run(unsigned int i)
    {
        while (true) {
            std::function<void()> f;
            for (unsigned int j = 0; j < n_; ++j) {
                if (queues_[(i + j) % n_].try_pop(f))
                    break;
            }
            if (!f && !queues_[i].pop(f))
                break;
            f();
        }
    }

    unsigned int const n_ = std::thread::hardware_concurrency();

    std::vector<std::thread> threads_;
    std::vector<concurrent_queue<std::function<void()>>> queues_{n_};
    std::atomic<unsigned int> index_{0u};
};
