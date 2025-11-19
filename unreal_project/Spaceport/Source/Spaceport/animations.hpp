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
        curve_(curve_object_path), apply_value_(std::move(apply_value)),
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
    animations() = default;
    animations(animations const &) = delete;
    animations & operator=(animations const &) = delete;

    bool need_update() const { return 0 < running_count_; }

    void start(adobe::name_t name)
    {
        for (auto & [name_, anim] : animations_) {
            if (name_ == name) {
                anim.start();
                break;
            }
        }
    }

    void update(float dt)
    {
        for (auto & [name, anim] : animations_) {
            if (anim.running())
                anim.update(dt);
        }
    }

    void insert(adobe::name_t name, animation anim)
    {
        animations_.emplace_back(name, std::move(anim));
        animations_.back().second.running_count_ = &running_count_;
    }

private:
    std::vector<std::pair<adobe::name_t, animation>> animations_;
    int running_count_ = 0;
};
