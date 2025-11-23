#pragma once

#include <boost/optional.hpp>

#include <vector>


template<typename T>
struct sparse_vector
{
    struct storage_element
    {
        int key_;
        T value_;
    };

    auto begin() const { return storage_.begin(); }
    auto end() const { return storage_.end(); }

    boost::optional<T const &> operator[](int i) const
    {
        auto const it =
            std::ranges::lower_bound(storage_, i, &storage_element::key_);
        if (it == storage_.end() || it->key_ != i)
            return {};
        return {storage_[i].value_};
    }

    void push_back() { ++push_back_i_; }
    void push_back(T && x)
    {
        storage_.emplace_back(push_back_i_++, std::move(x));
    }

    bool operator==(sparse_vector const &) const = default;

private:
    std::vector<storage_element> storage_;
    int push_back_i_ = 0;
};
