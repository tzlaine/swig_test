#include "name_generator.hpp"
#include "check.hpp"

#include <algorithm>


name_generator::name_generator(int order) : order_(order) { check(0 < order); }

name_generator::name_generator(
    std::vector<std::string> training_set, int order) :
    order_(order)
{
    train_on(std::move(training_set));
}

void name_generator::train_on(std::vector<std::string> training_set)
{
    // TODO: all strings in training set to lower case
    if (training_set_.empty()) {
        training_set_ = std::move(training_set);
    } else {
        auto const prev_size = training_set_.size();
        training_set_.resize(prev_size + training_set.size());
        std::ranges::move(training_set, training_set_.begin() + prev_size);
    }

    for (auto const & s : training_set_) {
        train_on(s);
    }
}

void name_generator::training_complete()
{
    normalize(prefixes_);
    for (auto && [prefix, productions] : all_productions_) {
        normalize(productions);
    }
    training_complete_ = true;
}

std::string name_generator::generate(detail::rng_state & state)
{
    check(!prefixes_.empty());

    if (!training_complete_)
        training_complete();

    std::string retval;
    std::string_view key = pick(prefixes_, state);
    retval += key;

    if (key.size() < order_)
        return retval;

    auto const end_of = [this](std::string const & s) {
        auto const first = s.data() + s.size() - order_;
        return std::string_view(first, order_);
    };

    while (!key.empty()) {
        key = end_of(retval);
        auto const it = all_productions_.find(key);
        if (it == all_productions_.end())
            break;
        key = pick(it->second, state);
        retval += key;
    }

    // TODO: result to title case
    return retval;
}

std::string_view name_generator::pick(
    std::vector<production> const & productions,
    detail::rng_state & state) const
{
    check(!productions.empty());
    float const p = random_unit_double(state);
    auto const it = std::ranges::lower_bound(
        productions, p, std::ranges::less{}, &production::p_);
    if (it == productions.end())
        return productions.back().str_;
    return it->str_;
}

void name_generator::train_on(std::string_view s)
{
    if (s.size() < order_) {
        insert(prefixes_, s);
        return;
    }

    insert(prefixes_, s.substr(0, order_));

    for (int i = 0, last = s.size() - order_; i < last; ++i) {
        // TODO: Break on grapheme noundaries here, and take order_ graphemes
        // for the key, one grapheme for next.
        std::string_view const curr = s.substr(i, order_);
        std::string_view next;
        if (i + order_ < s.size())
            next = s.substr(i + order_, 1);
        insert(all_productions_[curr], next);
    }
}

void name_generator::insert(
    std::vector<production> & productions, std::string_view s)
{
    // during_training, keep these arrays sorted
    auto it = std::ranges::lower_bound(
        productions, s, std::ranges::less{}, &production::str_);
    if (it == productions.end())
        productions.push_back({s, 1.0f});
    else if (it->str_ != s)
        productions.insert(it, {s, 1.0f});
    else
        it->p_ += 1.0f;
}

void name_generator::normalize(std::vector<production> & productions)
{
    float p_sum = 0.0f;
    for (auto const & prod : productions) {
        p_sum += prod.p_;
    }

    float p = 0.0f;
    for (auto & prod : productions) {
        // Normalize so that all probabilities sum to 1.
        prod.p_ /= p_sum;

        // Save this prob.
        float const this_p = prod.p_;

        // Add all previous probabilities to this one.
        prod.p_ += p;

        // Add this probability to p for the next iteration.
        p += this_p;
    }
}
