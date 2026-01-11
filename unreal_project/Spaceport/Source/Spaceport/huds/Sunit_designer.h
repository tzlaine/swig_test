#pragma once

#include "Shud_widget_base.h"

#include "game_instance.h"
#include "game_data.hpp"
#include "game_data_metadata.hpp"
#include "ui_defaults.h"
#include "utility.hpp" // TODO
#include "widgets/Sstyled_spin_box.h"
#include "widgets/Sstyled_text_block.h"

#include <CoreMinimal.h>
#include <Widgets/SBoxPanel.h>


class SVerticalBox;
struct nation_t;

class Sunit_designer : public Shud_widget_base
{
public:
    SLATE_BEGIN_ARGS(Sunit_designer)
        {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    void rebuild(nation_t const & nation, int design_id = -1);

private:
    template<typename Meta, typename TechMeta = int>
    void setting(Meta meta, int min_value = 0, TechMeta tech_meta = -1)
    {
        int max_value = 10000;
        if constexpr (std::same_as<TechMeta, int>) {
            if (0 < tech_meta)
                max_value = tech_meta;
        } else {
            check(min_value == 0);
            min_value = 0;
            if (meta.index_ ==
                detail::metadata<unit_design_t>::propulsion().index_) {
                min_value = 1;
            }
            max_value = nation_->*tech_meta.ptr_;
        }

        std::string name = std::format("unit_design_{}", meta.name_);

        UFont * font = ui_defaults().font_.Get();
        auto font_info = FSlateFontInfo(font, ui_defaults().font_size_ * 0.75);

        using spin_box_type = Sstyled_spin_box<int, 75>;

        // clang-format off
        left_vbox_->AddSlot().AutoHeight()[
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot().FillWidth(75)[
                SNew(Sstyled_text_block)
                .Text(loc_text(name))
                .Font(font_info)
            ]
            +SHorizontalBox::Slot().FillWidth(25)[
                SNew(spin_box_type)
                .MinValue(min_value)
                .Value_Lambda([this, p = meta.ptr_] { return design_.*p; })
                .MaxValue(max_value)
                .OnValueChanged_Lambda([this, p = meta.ptr_, i = meta.index_](int x) {
                    design_.*p = x;
                    design_changed(i, x);
                })
                .Font(font_info)
            ]
        ];
        // clang-format on
    }

    void design_changed(int field_index, int new_value);

    TSharedPtr<SVerticalBox> left_vbox_;
    TSharedPtr<SVerticalBox> right_vbox_;
    nation_t const * nation_;
    unit_design_t design_;
    int fighter_factor_strength_ = 0;
};
