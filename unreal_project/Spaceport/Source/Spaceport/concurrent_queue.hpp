#pragma once

#include <deque>
#include <thread>
#include <mutex>


template<typename T>
struct concurrent_queue
{
    bool pop(T & t)
    {
        std::unique_lock lock{m_};
        while (q_.empty() && !done_) {
            cv_.wait(lock);
        }
        if (q_.empty())
            return false;
        t = std::move(q_.front());
        q_.pop_front();
        return true;
    }

    bool try_pop(T & t)
    {
        std::unique_lock lock{m_, std::try_to_lock};
        if (!lock || q_.empty())
            return false;
        t = std::move(q_.front());
        q_.pop_front();
        return true;
    }

    template<typename U>
    bool try_push(U && u)
    {
        {
            std::unique_lock lock{m_, std::try_to_lock};
            if (!lock)
                return false;
            q_.emplace_back((U &&)u);
        }
        cv_.notify_one();
        return true;
    }

    void done()
    {
        {
            std::unique_lock lock{m_};
            done_ = true;
        }
        cv_.notify_all();
    }

private:
    std::deque<T> q_;
    std::mutex m_;
    std::condition_variable cv_;
    bool done_{false};
};
