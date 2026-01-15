#include "reason.hpp"

#if !defined(BUILD_FOR_TEST)
#include "game_instance.h"

#include <Internationalization/Text.h>
#endif


reason::reason(std::string_view s) : description_format_(s) {}

reason::reason(std::string_view s, int i) :
    description_format_(s), int_param_(i)
{}

reason::reason(std::string_view s, float f) :
    description_format_(s), float_param_(f)
{}

#if defined(BUILD_FOR_TEST)
std::string reason::text() const
{
    return {}; // TODO
}
#else
FText reason::text() const
{
    if (int_param_) {
        return FText::Format(
            loc_text(description_format_), FText::AsNumber(*int_param_));
    }
    if (float_param_) {
        FNumberFormattingOptions options;
        options.MaximumFractionalDigits = 2;
        return FText::Format(
            loc_text(description_format_),
            FText::AsNumber(*float_param_, &options));
    }
    return loc_text(description_format_);
}
#endif

std::string const & reason::description_format() const
{
    return description_format_;
}

bool reasons::empty() const { return reasons_.empty(); }

#if defined(BUILD_FOR_TEST)
std::string reasons::text() const
{
    return {}; // TODO
}
#else
FText reasons::text() const
{
    TArray<FText> texts;
    texts.SetNum(reasons_.size());
    std::ranges::transform(
        reasons_, texts.GetData(), [](auto e) { return e.text(); });
    return FText::Join(FText::FromString(TEXT("\n")), texts);
}
#endif

void reasons::insert(reason r, deduplicate dedup)
{
    if (dedup == deduplicate::yes)
        erase(r.description_format());
    reasons_.push_back(std::move(r));
}

void reasons::erase(std::string_view r)
{
    std::erase_if(reasons_, [r](auto const & e) {
        return e.description_format() == r;
    });
}
