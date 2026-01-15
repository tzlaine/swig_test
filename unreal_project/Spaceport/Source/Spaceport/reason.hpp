#pragma once

#include <optional>
#include <string>
#include <vector>


class FText;

// A user-facing reason for why something is happening, etc.
struct reason
{
    reason() = default;
    reason(std::string_view s);
    reason(std::string_view s, int i);
    reason(std::string_view s, float f);

#if defined(BUILD_FOR_TEST)
    std::string text() const;
#else
    FText text() const;
#endif
    std::string const & description_format() const;

private:
    std::string description_format_;
    std::optional<int> int_param_;
    std::optional<float> float_param_;
};


enum struct deduplicate { no, yes };

struct reasons
{
    bool empty() const;
#if defined(BUILD_FOR_TEST)
    std::string text() const;
#else
    FText text() const;
#endif

    void insert(reason r, deduplicate dedup = deduplicate::yes);
    void erase(std::string_view r);

private:
    std::vector<reason> reasons_;
};
