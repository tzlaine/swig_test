#pragma once

#include <format>
#include <type_traits>


namespace detail {
    inline unsigned int highest_bit_value(unsigned int v)
    {
        // From bit twiddling hacks, of course:
        // https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
        constexpr int MultiplyDeBruijnBitPosition[32] = {
            0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
            8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
        };
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        return 1 << MultiplyDeBruijnBitPosition[
            (unsigned int)(v * 0x07C4ACDDU) >> 27];
    }
}

template<typename E>
requires std::is_enum_v<E>
struct flags
{
    constexpr flags() = default;
    explicit constexpr flags(E e) : value_((unsigned int)e) {}
    explicit constexpr flags(unsigned int i) : value_(i) {}

    explicit operator bool() const { return !!value_; }
    operator unsigned int() const { return value_; }

    constexpr flags operator|(E e) const
    { return flags{value_ | flags(e).value_}; }
    constexpr flags operator|(flags other) const
    { return flags{value_ | other.value_}; }

    constexpr flags & operator|=(E e)
    { return *this |= flags(e); }
    constexpr flags & operator|=(flags other)
    { return *this = *this | other; }

    constexpr flags operator&(E e) const
    { return flags{value_ & flags(e).value_}; }
    constexpr flags operator&(flags other) const
    { return flags{value_ & other.value_}; }

    constexpr flags & operator&=(E e)
    { return *this &= flags(e); }
    constexpr flags & operator&=(flags other)
    { return *this = *this & other; }

    constexpr flags operator~() const
    { return flags{all_flags<E>().value_ & ~value_}; }

    constexpr E highest_flag_set() const
    {
        if (!value_)
            return (E)0;
        return (E)detail::highest_bit_value(value_);
    }

    constexpr bool operator==(flags const &) const = default;

private:
    unsigned int value_ = 0;
};

// Returns an |-ing of all the valid flags in the E enumeration.  Specialized
// definitions are generated.
template<typename E>
flags<E> all_flags();

template <typename E>
struct std::formatter<flags<E>> {
    template <typename Ctx>
    constexpr auto parse(Ctx & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(flags<E> e, Ctx & ctx) const {
        auto out = ctx.out();

        e &= all_flags<E>();

        bool first = true;
        E curr_flag = e.highest_flag_set();
        for (; (unsigned int)curr_flag;
             e &= ~curr_flag, curr_flag = e.highest_flag_set()) {
            if (first)
                first = false;
            else
                out = std::format_to(out, "|");
            out = std::format_to(out, "{}", curr_flag);
        }

        return out;
    }
};
