#pragma once

// See https://github.com/imneme/pcg-cpp
// Docs at: https://www.pcg-random.org/using-pcg-cpp.html

#ifndef __LITTLE_ENDIAN__
#define __LITTLE_ENDIAN__ 1
#endif
#include "pcg_random.hpp"

#include <random>
#include <ranges>


namespace detail {
    inline struct rng_state
    {
        rng_state() :
            pcg32_(pcg_extras::seed_seq_from<std::random_device>()),
            pcg64_(pcg_extras::seed_seq_from<std::random_device>()),
            unit_dist_(0.0, 1.0)
        {}

        pcg32 pcg32_;
        pcg64 pcg64_;
        std::uniform_real_distribution<double> unit_dist_;
    } g_rng_state;
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
