// Fill out your copyright notice in the Description page of Project Settings.


#include "main_menu.h"

void Umain_menu::NativeConstruct()
{
    Super::NativeConstruct();
    assert(continue_bn);
    assert(new_game_bn);
    assert(options_bn);
    assert(exit_bn);
    assert(game_state_actor);

    if (!current_game_available())
        continue_bn->SetIsEnabled(false);
    continue_bn->connect([] {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, TEXT("CLICK!"));
        // TODO: Hide this HUD if there is a current game; load the latest save-file otherwise.
    });

    new_game_bn->connect([this] {
        assert(game_state_actor.Get());
        RemoveFromParent();
        game_state_actor->new_game();
    });

    options_bn->connect([] {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, TEXT("One day, there will be options..."));
        // TODO
    });

    exit_bn->connect([this] {
#ifdef WITH_EDITOR
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, TEXT("Exit is disabled in the editor."));
        if (game_state_actor.Get()) {
            // TODO: Why doesn't this work?
            UKismetSystemLibrary::QuitGame(
                game_state_actor->GetWorld(),
                game_state_actor->GetWorld()->GetFirstPlayerController(),
                EQuitPreference::Quit,
                false);
        }
#else
        FGenericPlatformMisc::RequestExit(false);
#endif
    });
}

bool Umain_menu::current_game_available()
{
    return false; // TODO: return true if there is a game currently being played; otherwise, check for a latest save-file
}
