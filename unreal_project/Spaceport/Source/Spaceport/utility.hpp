#include <adobe/name.hpp>


namespace {
    float const meters = 100.0f;
}

template <typename T>
void call_real_soon(FTimerHandle& timer, T* t_ptr, void (T::*member_ptr)())
{
    t_ptr->GetWorldTimerManager().SetTimer(timer, t_ptr, member_ptr, 0.001, false);
}

namespace detail {
    char const * c_str(char const * str)
    { return str; }

    char const * c_str(name_t name)
    { return name.c_str(); }

    char const * c_str(std::string const & str)
    { return str.c_str(); }

    template <typename T>
    std::size_t strlen(T t)
    { return std::strlen(c_str(t)); }

    std::size_t strlen(std::string const & str)
    { return str.size(); }
}

template <typename ...T>
inline std::string make_string(T... t)
{
    std::string result;
    std::size_t size = 0;
    auto const dummy1 = {0, (size += std::strlen(detail::c_str(t)), 0)...};
    result.reserve(size);
    auto const dummy2 = {0, (result += detail::c_str(t), 0)...};
    return result;
}
