#pragma once

#include <ui_defaults.h>
#include <ui_util.h>

#include <memory>

#include <Widgets/Views/SListView.h>


template<typename ItemData>
class Sstyled_list_view : public SListView<ItemData>
{
public:
    using FArguments = typename SListView<ItemData>::FArguments;

    void Construct(FArguments const & args_)
    {
        style_ = std::make_unique<FTableViewStyle>(FTableViewStyle::GetDefault());
        // style_->BackgroundBrush = TODO;
        FSlateWidgetStyle s;
        typename SListView<ItemData>::FArguments args(args_);
        args.ListViewStyle(style_.get());
        SListView<ItemData>::Construct(args);
    }

private:
    std::unique_ptr<FTableViewStyle> style_;
};
