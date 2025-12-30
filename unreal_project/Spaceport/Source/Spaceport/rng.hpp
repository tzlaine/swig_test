#pragma once

// See https://github.com/imneme/pcg-cpp
// Docs at: https://www.pcg-random.org/using-pcg-cpp.html

#ifndef __LITTLE_ENDIAN__
#define __LITTLE_ENDIAN__ 1
#endif
#include "pcg_random.hpp"
#include "check.hpp"

#include <optional>
#include <random>
#include <ranges>


namespace detail {
    struct repeatable_seed_seq_from
    {
        using element_type = std::random_device::result_type;

        repeatable_seed_seq_from() : device_() {}
        repeatable_seed_seq_from(std::vector<element_type> seed) :
            device_(std::make_unique<std::random_device>()),
            seed_(std::move(seed)),
            use_seed_(true),
            next_it_(seed_.begin())
        {}
        repeatable_seed_seq_from(repeatable_seed_seq_from &&) = default;
        repeatable_seed_seq_from &
        operator=(repeatable_seed_seq_from &&) = default;

        template<typename I>
        void generate(I f, I l)
        {
            if (use_seed_) {
                check(
                    std::distance(f, l) <=
                    std::distance(next_it_, seed_.cend()));
                for (; f != l; ++f, ++next_it_) {
                    *f = *next_it_;
                }
            } else {
                std::generate(f, l, [this] {
                    auto const value = (*device_)();
                    seed_.push_back(value);
                    return value;
                });
            }
        }

        void reset()
        {
            next_it_ = seed_.begin();
            use_seed_ = true;
        }

        constexpr std::size_t size() const { return std::random_device::max(); }

        std::vector<element_type> const & seed() const { return seed_; }

    private:
        std::unique_ptr<std::random_device> device_;
        std::vector<element_type> seed_;
        bool use_seed_ = false;
        std::vector<element_type>::const_iterator next_it_;
    };

    inline repeatable_seed_seq_from make_seed_from(unsigned int arg)
    {
        std::vector<std::random_device::result_type> vec({arg});
        vec.resize(16);
        return repeatable_seed_seq_from(std::move(vec));
    }

    inline repeatable_seed_seq_from
    make_seed_from(unsigned int arg0, unsigned int arg1)
    {
        std::vector<std::random_device::result_type> vec({arg0, arg1});
        vec.resize(16);
        return repeatable_seed_seq_from(std::move(vec));
    }

    inline repeatable_seed_seq_from
    make_seed_from(unsigned int arg0, unsigned int arg1, unsigned int arg2)
    {
        std::vector<std::random_device::result_type> vec(
            {arg0, arg1, arg2, 0, 0, 0, arg2});
        vec.resize(16);
        return repeatable_seed_seq_from(std::move(vec));
    }

    inline repeatable_seed_seq_from make_seed_from(
        unsigned int arg0,
        unsigned int arg1,
        unsigned int arg2,
        unsigned int arg3)
    {
        std::vector<std::random_device::result_type> vec(
            {arg0, arg1, arg2, arg3, 0, 0, arg2, arg3});
        vec.resize(16);
        return repeatable_seed_seq_from(std::move(vec));
    }

    struct rng_state
    {
        rng_state(detail::repeatable_seed_seq_from && seeder = {}) :
            seeder_(std::move(seeder)), unit_dist_(0.0, 1.0)
        {
            pcg64_.seed(seeder_);
            seeder_.reset();
            pcg32_.seed(seeder_);
        }
        rng_state(rng_state &&) = default;
        rng_state & operator=(rng_state &&) = default;

        detail::repeatable_seed_seq_from seeder_;
        pcg32 pcg32_;
        pcg64 pcg64_;
        std::uniform_real_distribution<double> unit_dist_;
    };

    extern rng_state g_rng_state;

    inline rng_state rng_state_from(unsigned int arg)
    {
        return {detail::make_seed_from(arg)};
    }

    inline rng_state rng_state_from(unsigned int arg0, unsigned int arg1)
    {
        return {detail::make_seed_from(arg0, arg1)};
    }

    inline rng_state
    rng_state_from(unsigned int arg0, unsigned int arg1, unsigned int arg2)
    {
        return {detail::make_seed_from(arg0, arg1, arg2)};
    }

    inline rng_state rng_state_from(
        unsigned int arg0,
        unsigned int arg1,
        unsigned int arg2,
        unsigned int arg3)
    {
        return {detail::make_seed_from(arg0, arg1, arg2, arg3)};
    }
}

inline std::vector<unsigned int> const & rng_state_seed()
{
    return detail::g_rng_state.seeder_.seed();
}

inline void reset_rng_state(std::vector<unsigned int> seed)
{
    detail::g_rng_state = detail::rng_state(detail::repeatable_seed_seq_from(std::move(seed)));
}

template<template<typename> typename DistTemplate, typename T>
T random_number(
    DistTemplate<T> & dist, detail::rng_state & state = detail::g_rng_state)
{
    if constexpr (sizeof(T) <= 4) {
        return dist(state.pcg32_);
    } else {
        return dist(state.pcg64_);
    }
}

inline int
random_int(int lo, int hi, detail::rng_state & state = detail::g_rng_state)
{
    std::uniform_int_distribution<int> dist(lo, hi);
    return random_number(dist, state);
}

inline double random_double(
    double lo, double hi, detail::rng_state & state = detail::g_rng_state)
{
    std::uniform_real_distribution<double> dist(lo, hi);
    return random_number(dist, state);
}

inline double
random_unit_double(detail::rng_state & state = detail::g_rng_state)
{
    return random_number(state.unit_dist_, state);
}

template<
    template<typename> typename DistTemplate,
    typename T,
    std::ranges::range R>
std::ranges::borrowed_iterator_t<R> random_numbers(
    DistTemplate<T> & dist,
    R && r,
    detail::rng_state & state = detail::g_rng_state)
{
    auto f = std::ranges::begin(r);
    auto const l = std::ranges::end(r);
    for (; f != l; ++f) {
        if constexpr (sizeof(T) <= 4) {
            *f = dist(state.pcg32_);
        } else {
            *f = dist(state.pcg64_);
        }
    }
    return f;
}

template<std::ranges::range R>
std::ranges::borrowed_iterator_t<R> random_ints(
    int lo, int hi, R && r, detail::rng_state & state = detail::g_rng_state)
{
    std::uniform_int_distribution<int> dist(lo, hi);
    return random_numbers(dist, (R &&)r, state);
}

template<std::ranges::range R>
std::ranges::borrowed_iterator_t<R> random_doubles(
    double lo,
    double hi,
    R && r,
    detail::rng_state & state = detail::g_rng_state)
{
    return random_numbers(state.unit_dist_, (R &&)r, state);
}
