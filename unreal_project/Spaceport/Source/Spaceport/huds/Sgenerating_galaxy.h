#pragma once

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>
#include <Widgets/SBoxPanel.h>

class Sstyled_progress_bar;

class Sgenerating_galaxy : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Sgenerating_galaxy)
        {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    void percent_complete(int p);

private:
    TSharedPtr<Sstyled_progress_bar> bar_;
};
