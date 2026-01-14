#include "Sunit_designer.h"

#include "constants.hpp"
#include "game_data_metadata.hpp"
#include "game_instance.h"
#include "lua.hpp"
#include "widgets/Sstyled_rich_text_block.h"
#include "widgets/Sstyled_button.h"

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
#include <Widgets/SOverlay.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Layout/SBackgroundBlur.h>
#include <Widgets/Layout/SConstraintCanvas.h>
#include <Widgets/Layout/SScaleBox.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace {
    int const margin = 5;

    FText
    make_resource_cost_text(cost_t const & cost, std::string_view separator)
    {
        std::string format_str;
        std::vector<float> values;
        detail::metadata<cost_t>::foreach_member([&](auto meta) {
            float const value = cost.*meta.ptr_;
            if (value == n_a)
                return;
            std::string_view sep = "";
            if (!format_str.empty())
                sep = separator;
            format_str += std::format(
                "{} <img src=\"{}\"/> {{{}}}",
                sep,
                meta.name_,
                values.size());
            values.push_back(value);
        });
        FText format_text = FText::FromString(FString(format_str.c_str()));
        FNumberFormattingOptions options;
        options.MaximumFractionalDigits = 2;
        switch (values.size()) {
        default:
        case 0u: return FText();
        case 1u:
            return FText::Format(
                format_text, FText::AsNumber(values[0], &options));
        case 2u:
            return FText::Format(
                format_text,
                FText::AsNumber(values[0], &options),
                FText::AsNumber(values[1], &options));
        case 3u:
            return FText::Format(
                format_text,
                FText::AsNumber(values[0], &options),
                FText::AsNumber(values[1], &options),
                FText::AsNumber(values[2], &options));
        case 4u:
            return FText::Format(
                format_text,
                FText::AsNumber(values[0], &options),
                FText::AsNumber(values[1], &options),
                FText::AsNumber(values[2], &options),
                FText::AsNumber(values[3], &options));
        case 5u:
            return FText::Format(
                format_text,
                FText::AsNumber(values[0], &options),
                FText::AsNumber(values[1], &options),
                FText::AsNumber(values[2], &options),
                FText::AsNumber(values[3], &options),
                FText::AsNumber(values[4], &options));
        case 6u:
            return FText::Format(
                format_text,
                FText::AsNumber(values[0], &options),
                FText::AsNumber(values[1], &options),
                FText::AsNumber(values[2], &options),
                FText::AsNumber(values[3], &options),
                FText::AsNumber(values[4], &options),
                FText::AsNumber(values[5], &options));
        case 7u:
            return FText::Format(
                format_text,
                FText::AsNumber(values[0], &options),
                FText::AsNumber(values[1], &options),
                FText::AsNumber(values[2], &options),
                FText::AsNumber(values[3], &options),
                FText::AsNumber(values[4], &options),
                FText::AsNumber(values[5], &options),
                FText::AsNumber(values[6], &options));
        }
    }

    template<typename F>
        requires std::is_invocable_r_v<cost_t, F>
    auto resource_cost_text(F && f, std::string separator = " ")
    {
        return [f = (F &&)f, separator = std::move(separator)] {
            cost_t const & cost = f();
            return make_resource_cost_text(cost, separator);
        };
    }
}

void Sunit_designer::Construct(FArguments const & args)
{
    // clang-format off
    ChildSlot[SNew(SBackgroundBlur).BlurStrength(5.0f)[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.2, 0.1, 0.8, 0.9))[
            SNew(SVerticalBox)

            +SVerticalBox::Slot().FillHeight(90)[
                SNew(SHorizontalBox)
                +SHorizontalBox::Slot().FillWidth(50)[
                    ASSIGN_STYLED_SCROLL_BOX(left_vbox_)
                ]
                +SHorizontalBox::Slot().FillWidth(50)[
                    ASSIGN_STYLED_SCROLL_BOX(right_vbox_)
                ]
            ]

            +SVerticalBox::Slot()
            .FillHeight(10).VAlign(VAlign_Top).HAlign(HAlign_Fill)[
                SNew(SHorizontalBox)

                +SHorizontalBox::Slot().FillWidth(75)[
                    SNew(SVerticalBox)

                    +SVerticalBox::Slot().FillHeight(50)[
                        SNew(SHorizontalBox)

                        +SHorizontalBox::Slot().AutoWidth()[
                            SNew(Sstyled_rich_text_block)
                            .Text(loc_text("estimated_design_cost"))
                        ]

                        +SHorizontalBox::Slot().AutoWidth()[
                            SNew(Sstyled_rich_text_block)
                            .Text_Lambda([this] {
                                FNumberFormattingOptions options;
                                options.MaximumFractionalDigits = 2;
                                float est_monthly_cost = 25.0f;
                                float est_months = 4.0f;
                                float const est_total =
                                    est_monthly_cost * est_months;
                                return FText::Format(
                                    loc_text("estimated_design_cost_value"),
                                    FText::AsNumber(est_total, &options),
                                    FText::AsNumber(est_monthly_cost, &options),
                                    FText::AsNumber(est_months, &options));
                            })
                        ]
                    ]

                    +SVerticalBox::Slot().FillHeight(50)[
                        SNew(SHorizontalBox)

                        +SHorizontalBox::Slot().AutoWidth()[
                            SNew(Sstyled_rich_text_block)
                            .Text(loc_text("estimated_unit_cost"))
                        ]

                        +SHorizontalBox::Slot().FillWidth(75)[
                            SNew(Sstyled_rich_text_block)
                            .Text_Lambda(resource_cost_text([this] {
                                cost_t retval =
                                    call_lua_func("unit_cost", design_);
                                detail::metadata<cost_t>::foreach_member(
                                    [&](auto meta) {
                                        if (retval.*meta.ptr_ == 0.0f)
                                            retval.*meta.ptr_ = n_a;
                                    });
                                return retval;
                            }, "   "))
                        ]
                    ]
                ]

                +SHorizontalBox::Slot().FillWidth(25).VAlign(VAlign_Center)[
                    SAssignNew(start_design_button_, Sstyled_button)
                    .Text(loc_text("start_design"))
                    // TODO: Need a custom tooltip for sure.  The current
                    // default is unreadably small.
                    .ToolTipText_Lambda([this] {
                        if (reasons_design_is_broken_.empty())
                            return FText();
                        TArray<FText> errors;
                        errors.SetNum(reasons_design_is_broken_.size());
                        std::ranges::transform(
                            reasons_design_is_broken_,
                            errors.GetData(),
                            [](auto e) { return loc_text(e); });
                        return FText::Join(
                            FText::FromString(TEXT("\n")), errors);
                    })
                ]
            ]
        ]
    ]];
    // clang-format on
}

void Sunit_designer::rebuild(nation_t const & nation, int design_id)
{
    nation_ = &nation;

    // TODO: Need some other way of getting to a starting design....
    design_.hull = 100;
    design_.armor = 0;
    design_.propulsion = 1;
    design_.weapons = 1;
    design_.shields = 1;
    design_.detection = 1;
    design_.stealth = 1;
    design_.fuel = 2;
    design_.water = 2;
    design_.supplies = 2;
    design_.rounds = 1;
    design_.missiles = 1;
    design_.fighters = 0; // disabled/not even shown, based on tech?
    design_.cargo = 0;

    UFont * font = ui_defaults().font_.Get();

    // left side: chosen settings

    // clang-format off
    left_vbox_->AddSlot()[
        SNew(Sstyled_text_block)
        .Text(loc_text("unit_design_hull_and_armor"))
        .Font(FSlateFontInfo(font, ui_defaults().font_size_))
    ];
    // clang-format on

    int const max_hull = call_lua_func("max_hull_space", *nation_);
    setting(detail::metadata<unit_design_t>::hull(), 1, max_hull);
    setting(
        detail::metadata<unit_design_t>::armor(),
        1,
        int(max_hull * max_armor_per_hull_point));

    // clang-format off
    left_vbox_->AddSlot().Padding(0, 10, 0, 0)[
        SNew(Sstyled_text_block)
        .Text(loc_text("unit_design_equipment"))
        .Font(FSlateFontInfo(font, ui_defaults().font_size_))
    ];
    // clang-format on

    setting(
        detail::metadata<unit_design_t>::propulsion(),
        0,
        detail::metadata<nation_t>::propulsion_tech());
    setting(
        detail::metadata<unit_design_t>::weapons(),
        0,
        detail::metadata<nation_t>::weapons_tech());
    setting(
        detail::metadata<unit_design_t>::shields(),
        0,
        detail::metadata<nation_t>::shields_tech());
    setting(
        detail::metadata<unit_design_t>::detection(),
        0,
        detail::metadata<nation_t>::detection_tech());
    setting(
        detail::metadata<unit_design_t>::stealth(),
        0,
        detail::metadata<nation_t>::stealth_tech());

    // clang-format off
    left_vbox_->AddSlot()[
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot().FillWidth(75).Padding(0, 10, 0, 0)[
            SNew(Sstyled_text_block)
            .Text(loc_text("unit_design_non_equipment_space"))
            .Font(font_info())
        ]
        +SHorizontalBox::Slot().FillWidth(25).Padding(0, 10, 0, 0)[
            SNew(Sstyled_text_block)
            .Text_Lambda([this] {
                int const space =
                    call_lua_func("unit_nonequipment_space", design_);
                return FText::Format(
                    loc_text("unit_design_value_non_equipment_space"),
                    FText::AsNumber(space));
            })
            .Font(font_info())
        ]
    ];
    // clang-format on

    // clang-format off
    left_vbox_->AddSlot().Padding(0, 10, 0, 0)[
        SNew(Sstyled_text_block)
        .Text(loc_text("unit_design_storage_space"))
        .Font(FSlateFontInfo(font, ui_defaults().font_size_))
    ];
    // clang-format on

    if (fighter_factor_strength_ =
            call_lua_func("base_fighter_factor_strength", *nation_)) {
        // clang-format off
        left_vbox_->AddSlot().Padding(0, 10, 0, 0)[
            SNew(Sstyled_text_block)
            .Text(loc_text("unit_design_fighters"))
            .Font(FSlateFontInfo(font, ui_defaults().font_size_))
        ];
        // clang-format on
        setting(detail::metadata<unit_design_t>::fighters(), 0);
    }

    setting(detail::metadata<unit_design_t>::fuel(), 1);
    setting(detail::metadata<unit_design_t>::water(), 1);
    setting(detail::metadata<unit_design_t>::supplies(), 1);
    setting(detail::metadata<unit_design_t>::rounds(), 0);
    setting(detail::metadata<unit_design_t>::missiles(), 0);
    setting(detail::metadata<unit_design_t>::cargo(), 0);

    // clang-format off
    left_vbox_->AddSlot()[
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot().FillWidth(75).Padding(0, 10, 0, 0)[
            SNew(Sstyled_text_block)
            .Text(loc_text("unit_design_unused_space"))
            .Font(font_info())
        ]
        +SHorizontalBox::Slot().FillWidth(25).Padding(0, 10, 0, 0)[
            SNew(Sstyled_text_block)
            .ColorAndOpacity_Lambda([this] {
                int const space = call_lua_func("unit_unused_space", design_);
                if (space < 0 ) {
                    insert_reason(reasons_design_is_broken_,
                                  "broken_design_not_enough_space");
                    return FSlateColor(ui_defaults().error_text_color_);
                }
                erase_reason(reasons_design_is_broken_,
                             "broken_design_not_enough_space");
                return FSlateColor(FLinearColor::White);
            })
            .Text_Lambda([this] {
                int const space = call_lua_func("unit_unused_space", design_);
                return FText::Format(
                    loc_text("unit_design_value_unused_space"),
                    FText::AsNumber(space));
            })
            .Font(font_info())
        ]
    ];
    // clang-format on

    // right side: properties of the design resulting from the chosen settings

    // clang-format off
    right_vbox_->AddSlot()[
        SNew(Sstyled_text_block)
        .Text(loc_text("unit_design_ideal_design"))
        .Font(FSlateFontInfo(font, ui_defaults().font_size_))
    ];
    // clang-format on

    property("crew", use_nation_t::no);
    property("subspace_speed", use_nation_t::no);
    property("subspace_range", use_nation_t::no);
    property("mass", use_nation_t::no);
    property("max_acceleration", use_nation_t::no);
    property("sustained_acceleration", use_nation_t::yes);
    property("days_at_sustained_acceleration", use_nation_t::yes);
    property("months_of_water", use_nation_t::no);
    property("months_of_supplies", use_nation_t::no);
    property("pd_volleys", use_nation_t::no, [this] {
        erase_reason(warnings_about_design_, "design_warning_no_rounds");
        if (0 < design_.weapons && design_.rounds == 0) {
            if (design_.missiles == 0) {
                insert_reason(
                    reasons_design_is_broken_, "broken_design_no_ammo");
                return FSlateColor(ui_defaults().error_text_color_);
            } else {
                insert_reason(
                    warnings_about_design_, "design_warning_no_rounds");
                return FSlateColor(ui_defaults().warning_text_color_);
            }
        } else {
            erase_reason(reasons_design_is_broken_, "broken_design_no_ammo");
        }
        return FSlateColor(FLinearColor::White);
    });
    property("missile_volleys", use_nation_t::no, [this] {
        erase_reason(warnings_about_design_, "design_warning_no_missles");
        if (0 < design_.weapons && design_.missiles == 0) {
            if (design_.rounds == 0) {
                insert_reason(
                    reasons_design_is_broken_, "broken_design_no_ammo");
                return FSlateColor(ui_defaults().error_text_color_);
            } else {
                insert_reason(
                    warnings_about_design_, "design_warning_no_missles");
                return FSlateColor(ui_defaults().warning_text_color_);
            }
        } else {
            erase_reason(reasons_design_is_broken_, "broken_design_no_ammo");
        }
        return FSlateColor(FLinearColor::White);
    });
}

FSlateFontInfo Sunit_designer::font_info() const
{
    UFont * font = ui_defaults().font_.Get();
    return FSlateFontInfo(font, ui_defaults().font_size_ * 0.75);
}

void Sunit_designer::design_changed(int field_index, int new_value)
{
    switch (field_index) {
    case detail::metadata<unit_design_t>::hull().index_: break;

    case detail::metadata<unit_design_t>::armor().index_:
        design_.armor = std::min(
            design_.armor, int(design_.hull * max_armor_per_hull_point));
        break;

    case detail::metadata<unit_design_t>::propulsion().index_: break;

    case detail::metadata<unit_design_t>::weapons().index_:
        if (new_value == 0) {
            design_.rounds = 0;
            design_.missiles = 0;
        }
        break;

    case detail::metadata<unit_design_t>::shields().index_: break;
    case detail::metadata<unit_design_t>::detection().index_: break;
    case detail::metadata<unit_design_t>::stealth().index_: break;
    case detail::metadata<unit_design_t>::fuel().index_: break;
    case detail::metadata<unit_design_t>::water().index_: break;
    case detail::metadata<unit_design_t>::supplies().index_: break;

    case detail::metadata<unit_design_t>::rounds().index_:
    case detail::metadata<unit_design_t>::missiles().index_:
        if (design_.weapons == 0) {
            design_.rounds = 0;
            design_.missiles = 0;
        }
        break;

    case detail::metadata<unit_design_t>::fighters().index_: break;
    case detail::metadata<unit_design_t>::cargo().index_: break;
    }
}

void Sunit_designer::property(
    std::string_view name,
    use_nation_t use_nation,
    std::function<FSlateColor()> color_func)
{
    std::string label_name = "unit_design_";
    label_name += name;
    std::string value_name = "unit_design_value_";
    value_name += name;
    std::string func_name = "unit_";
    func_name += name;

    if (!color_func)
        color_func = [] { return FSlateColor(FLinearColor::White); };

    // clang-format off
    right_vbox_->AddSlot()[
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot().FillWidth(75).Padding(0, 10, 0, 0)[
            SNew(Sstyled_text_block)
            .Text(loc_text(label_name))
            .Font(font_info())
        ]
        +SHorizontalBox::Slot().FillWidth(25).Padding(0, 10, 0, 0)[
            SNew(Sstyled_text_block)
            .Justification(ETextJustify::Right)
            .Text_Lambda([=, this] {
                FNumberFormattingOptions options;
                options.MaximumFractionalDigits = 2;
                double const value = use_nation == use_nation_t::yes ?
                    call_lua_func(func_name, design_, *nation_) :
                    call_lua_func(func_name, design_);
                return FText::Format(
                    loc_text(value_name),
                    FText::AsNumber(value, &options));
            })
            .ColorAndOpacity_Lambda(color_func)
            .Font(font_info())
        ]
    ];
    // clang-format on
}

void Sunit_designer::insert_reason(
    std::vector<std::string_view> & reasons, std::string_view reason)
{
    erase_reason(reasons, reason);
    reasons.push_back(reason);
    if (!reasons_design_is_broken_.empty())
        start_design_button_->SetEnabled(false);
}

void Sunit_designer::erase_reason(
    std::vector<std::string_view> & reasons, std::string_view reason)
{
    std::erase(reasons, reason);
    if (reasons_design_is_broken_.empty())
        start_design_button_->SetEnabled(true);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
