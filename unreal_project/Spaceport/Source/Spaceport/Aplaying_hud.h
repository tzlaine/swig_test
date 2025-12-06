#pragma once

#include "Ahud_base.h"

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Aplaying_hud.generated.h"


class Amap_pawn_base;
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
    void remove_game_setup_widget();

    void show_generating_galaxy();
    void generating_percent_update(int p);
    void remove_generating_widget();

    void set_selection_box_first(FVector2D first);
    void set_selection_box_last(FVector2D last);
    TArray<Amap_pawn_base *> & selected_in_box();

    void BeginPlay() override;
    void EndPlay(EEndPlayReason::Type reason) override;
    void DrawHUD() override;

private:
    void allocate_widgets();

    TSharedPtr<Sgame_setup> game_setup_;
    TSharedPtr<Sgenerating_galaxy> generating_galaxy_;
    int generating_progress_ = 0;

    FVector2D selection_box_first_;
    FVector2D selection_box_last_;
    TArray<Amap_pawn_base *> selected_pawns_;
};
