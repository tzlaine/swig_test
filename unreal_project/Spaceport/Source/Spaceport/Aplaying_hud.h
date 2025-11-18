#pragma once

#include "Ahud_base.h"

#include <vector>

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Aplaying_hud.generated.h"


class Smain_menu;
class Sgame_setup;
class Sgenerating_galaxy;
class Shud_widget_base;

UCLASS()
class Aplaying_hud : public Ahud_base
{
    GENERATED_BODY()

public:
    Aplaying_hud(FObjectInitializer const & init);

    void saves_list(TArray<FString> const & saves) override;
    void saves_changed(TArray<Ffile_change> const & changes) override;
    void escape_pressed() override;

    void show_main_menu();
    void hide_main_menu();

    void show_game_setup();
    void hide_game_setup();

    void show_generating_galaxy();
    void hide_generating_galaxy();
    void generating_percent_update(int p);

protected:
    void BeginPlay() override;
    void EndPlay(EEndPlayReason::Type reason) override;

    void allocate_widgets();
    void show_modal(Shud_widget_base * widget);
    void hide_modal(Shud_widget_base * widget);

    TSharedPtr<Smain_menu> main_menu_;
    TSharedPtr<Sgame_setup> game_setup_;
    TSharedPtr<Sgenerating_galaxy> generating_galaxy_;
    int generating_progress_ = 0;

    std::vector<Shud_widget_base *> modal_stack_;
};
