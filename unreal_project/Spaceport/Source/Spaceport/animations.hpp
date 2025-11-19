#pragma once

#include <functional>
#include <vector>
#include <cassert>

#include <adobe/name.hpp>

#include <CoreMinimal.h>


struct animations;

struct animation
{
    animation(
        FString const & curve_object_path,
        float dur,
        std::function<void(float)> apply_value) :
        curve_(curve_object_path),
        apply_value_(std::move(apply_value)),
        dur_(dur)
    {
        assert(0.0f < dur);
    }

    bool running() const { return 0.0f <= t_; }

    void start()
    {
        t_ = 0.0f;
        if (running_count_)
            ++*running_count_;
        curve_.LoadSynchronous();
        apply_value_(0.0f);
    }

    void update(float dt)
    {
        t_ += dt;
        float const alpha = std::min(t_ / dur_, 1.0f);
        float const value = curve_->GetFloatValue(alpha);
        apply_value_(value);
        if (dur_ < t_) {
            t_ = -1.0f;
            if (running_count_)
                --*running_count_;
        }
    }

private:
    TSoftObjectPtr<UCurveFloat> curve_;
    std::function<void(float)> apply_value_;
    int * running_count_ = nullptr;
    float t_ = -1.0f;
    float dur_ = 1.0f;

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

        animations * this_ = nullptr;
        int just_inserted_index_ = 0;
    };
    friend then_enabler;

public:
    animations() = default;
    animations(animations const &) = delete;
    animations & operator=(animations const &) = delete;

    bool need_update() const { return 0 < running_count_; }

    void start(adobe::name_t name)
    {
        using namespace adobe::literals;
        assert(name != ""_name);
        for (auto & [name_, anim, _] : animations_) {
            if (name_ == name) {
                anim.start();
                break;
            }
        }
    }

    void update(float dt)
    {
        for (auto & [name, anim, next] : animations_) {
            if (anim.running()) {
                anim.update(dt);
                if (!anim.running() && 0 < next)
                    animations_[next].anim_.start();
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
    };

    std::vector<animation_info> animations_;
    int running_count_ = 0;
};
