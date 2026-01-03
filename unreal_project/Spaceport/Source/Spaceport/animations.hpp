#pragma once

#include <functional>
#include <vector>

#include <adobe/name.hpp>

#include <CoreMinimal.h>


struct animations;

enum struct animation_kind {
    linear,
    linear_quadratic,
    linear_cubic,
    smooth,
    smooth_quadratic,
    smooth_cubic
};

inline float animate(float alpha, animation_kind method)
{
    float power = 1.0;
    switch (method) {
    default:
    case animation_kind::linear: return alpha;
    case animation_kind::linear_quadratic: return alpha * alpha;
    case animation_kind::linear_cubic: return alpha * alpha * alpha;
    case animation_kind::smooth:
    case animation_kind::smooth_quadratic: power = 2.0f;
    case animation_kind::smooth_cubic:
        power = 3.0f;
        alpha = FMath::SmoothStep(0.0f, 1.0f, alpha);
        return FMath::Pow(alpha, power);
    }
}

struct animation
{
    animation(
        animation_kind method,
        float dur,
        std::function<void(float)> apply_value) :
        apply_value_(std::move(apply_value)), dur_(dur), method_(method)
    {
        check(0.0f < dur);
    }

    bool running() const { return 0.0f <= t_; }

    void start()
    {
        t_ = 0.0f;
        if (running_count_)
            ++*running_count_;
        apply_value_(0.0f);
    }

    void tick(float dt)
    {
        t_ += dt;
        float const alpha = std::min(t_ / dur_, 1.0f);
        float const value = animate(alpha, method_);
        apply_value_(value);
        if (dur_ < t_) {
            t_ = -1.0f;
            if (running_count_)
                --*running_count_;
        }
    }

private:
    std::function<void(float)> apply_value_;
    int * running_count_ = nullptr;
    float t_ = -1.0f;
    float dur_ = 1.0f;
    animation_kind method_ = animation_kind::linear;

    friend animations;
};

struct animations
{
private:
    struct then_enabler
    {
        void then(animation anim)
        {
            using namespace adobe::literals;
            int const inserting_at_index = (int)this_->animations_.size();
            this_->insert(""_name, std::move(anim));
            this_->animations_[just_inserted_index_].run_next_index_ =
                inserting_at_index;
        }
        void then(std::function<void()> f)
        {
            int const inserting_at_index = (int)this_->animations_.size();
            this_->animations_[just_inserted_index_].next_f_ = std::move(f);
        }

        animations * this_ = nullptr;
        int just_inserted_index_ = 0;
    };
    friend then_enabler;

public:
    animations() = default;
    animations(animations const &) = delete;
    animations & operator=(animations const &) = delete;

    bool need_tick() const { return 0 < running_count_; }

    void start(adobe::name_t name)
    {
        using namespace adobe::literals;
        check(name != ""_name);
        for (auto & [name_, anim, _0, _1] : animations_) {
            if (name_ == name) {
                anim.start();
                break;
            }
        }
    }

    void tick(float dt)
    {
        for (auto & [name, anim, next_index, f] : animations_) {
            if (anim.running()) {
                anim.tick(dt);
                if (!anim.running()) {
                    if (0 <= next_index)
                        animations_[next_index].anim_.start();
                    else if (f)
                        f();
                }
            }
        }
    }

    then_enabler insert(adobe::name_t name, animation anim)
    {
        int const inserting_at_index = (int)animations_.size();
        animations_.push_back(animation_info{name, std::move(anim), -1});
        animations_.back().anim_.running_count_ = &running_count_;
        return {this, inserting_at_index};
    }

private:
    struct animation_info
    {
        adobe::name_t name_;
        animation anim_;
        int run_next_index_ = -1;
        std::function<void()> next_f_;
    };

    std::vector<animation_info> animations_;
    int running_count_ = 0;
};
