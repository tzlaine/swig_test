#pragma once

#include "Ahud_base.h"

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Aplaying_hud.generated.h"


class Sgame_setup;
class Sgenerating_galaxy;
class Smain_menu;
class Shud_widget_base;

UCLASS()
class Aplaying_hud : public Ahud_base
{
    GENERATED_BODY()

public:
    Aplaying_hud(FObjectInitializer const & init);

    void saves_list(TArray<FString> const & saves) override;
    void saves_changed(TArray<Ffile_change> const & changes) override;

    void show_game_setup();
    void hide_game_setup();

    void show_generating_galaxy();
    void hide_generating_galaxy();
    void generating_percent_update(int p);

protected:
    void BeginPlay() override;
    void EndPlay(EEndPlayReason::Type reason) override;

    void allocate_widgets();

    TSharedPtr<Sgame_setup> game_setup_;
    TSharedPtr<Sgenerating_galaxy> generating_galaxy_;
    int generating_progress_ = 0;
};
