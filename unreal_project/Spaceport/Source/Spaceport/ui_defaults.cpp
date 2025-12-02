#include "ui_defaults.h"

#include "Aplayer_controller_base.h"
#include "utility.hpp"
#include "widgets/Sstyled_text_block.h"


Uui_defaults_t::Uui_defaults_t()
{
    table_row_style_.SetOddRowBackgroundBrush(
        FSlateColorBrush(FLinearColor::Transparent));
    table_row_style_.SetEvenRowBackgroundBrush(
        FSlateColorBrush(FLinearColor::Transparent));
}

TSharedRef<ITableRow> Uui_defaults_t::make_row(
    TSharedPtr<FString> text, TSharedRef<STableViewBase> const & table) const
{
    return SNew(STableRow<TSharedPtr<FString>>, table)
        .Style(&table_row_style_)
        .Padding(2)[SNew(Sstyled_text_block).Text(FText::FromString(*text))];
}

Uui_defaults_t const & ui_defaults()
{
    auto * pc = player_controller_base();
    check(pc);
    return pc->ui_defaults();
}
