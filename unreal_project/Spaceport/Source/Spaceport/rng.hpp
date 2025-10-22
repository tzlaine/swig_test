#pragma once

// See https://github.com/imneme/pcg-cpp
// Docs at: https://www.pcg-random.org/using-pcg-cpp.html

#ifndef __LITTLE_ENDIAN__
#define __LITTLE_ENDIAN__ 1
#endif
#include "pcg_random.hpp"

#include <optional>
#include <random>
#include <ranges>


namespace detail {
    struct repeatable_seed_seq_from
    {
        using element_type = std::random_device::result_type;

        repeatable_seed_seq_from() : device_(std::in_place) {}
        repeatable_seed_seq_from(std::vector<element_type> seed) :
            device_(), seed_(std::move(seed)), use_seed_(true), next_it_(seed_.begin())
        {}

        repeatable_seed_seq_from &
        operator=(repeatable_seed_seq_from && other)
        {
            seed_ = std::move(other.seed_);
            use_seed_ = other.use_seed_;
            next_it_ = other.next_it_;
            return *this;
        }

        template<typename I>
        void generate(I f, I l)
        {
            if (use_seed_) {
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

        constexpr std::size_t size() const
        {
            return std::random_device::max();
        }

        std::vector<element_type> const & seed() const
        {
            return seed_;
        }

    private:
        std::optional<std::random_device> device_;
        std::vector<element_type> seed_;
        bool use_seed_ = false;
        std::vector<element_type>::const_iterator next_it_;
    };

    inline struct rng_state
    {
        rng_state() :
            seeder_(), pcg32_(seeder_), pcg64_(seeder_), unit_dist_(0.0, 1.0)
        {}

        repeatable_seed_seq_from seeder_;
        pcg32 pcg32_;
        pcg64 pcg64_;
        std::uniform_real_distribution<double> unit_dist_;
    } g_rng_state;
}

inline std::vector<unsigned int> const & rng_state_seed()
{
    return detail::g_rng_state.seeder_.seed();
}

inline void reset_rng_state(std::vector<unsigned int> seed)
{
    detail::g_rng_state.seeder_ =
        detail::repeatable_seed_seq_from(std::move(seed));
    detail::g_rng_state.pcg32_ = pcg32(detail::g_rng_state.seeder_);
    detail::g_rng_state.pcg64_ = pcg64(detail::g_rng_state.seeder_);
    detail::g_rng_state.unit_dist_ = std::uniform_real_distribution<double>();
}

template<template<typename> typename DistTemplate, typename T>
T random_number(DistTemplate<T> & dist)
{
    if constexpr (sizeof(T) <= 4) {
        return dist(detail::g_rng_state.pcg32_);
    } else {
        return dist(detail::g_rng_state.pcg64_);
    }
}

inline int random_int(int lo, int hi)
{
    std::uniform_int_distribution<int> dist(lo, hi);
    return random_number(dist);
}

inline double random_double(double lo, double hi)
{
    std::uniform_real_distribution<double> dist(lo, hi);
    return random_number(dist);
}

inline double random_unit_double()
{
    return random_number(detail::g_rng_state.unit_dist_);
}

template<template<typename> typename DistTemplate, typename T,
         std::ranges::range R>
std::ranges::borrowed_iterator_t<R>
random_numbers(DistTemplate<T> & dist, R && r)
{
    auto f = std::ranges::begin(r);
    auto const l = std::ranges::end(r);
    for (; f != l; ++f) {
        if constexpr (sizeof(T) <= 4) {
            *f = dist(detail::g_rng_state.pcg32_);
        } else {
            *f = dist(detail::g_rng_state.pcg64_);
        }
    }
    return f;
}

template<std::ranges::range R>
std::ranges::borrowed_iterator_t<R> random_ints(int lo, int hi, R && r)
{
    std::uniform_int_distribution<int> dist(lo, hi);
    return random_numbers(dist, (R &&)r);
}

template<std::ranges::range R>
std::ranges::borrowed_iterator_t<R>
random_doubles(double lo, double hi, R && r)
{
    return random_numbers(detail::g_rng_state.unit_dist_, (R &&)r);
}
