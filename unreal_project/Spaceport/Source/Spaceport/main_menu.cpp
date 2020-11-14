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

    new_game_bn->connect([] {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, TEXT("Coming soon!"));
        // TODO
    });

    options_bn->connect([] {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, TEXT("One day, there will be options..."));
        // TODO
    });

    exit_bn->connect([this] {
        // TODO: We don't have a reference to the world in this part of the code!
        // UKismetSystemLibrary::QuitGame(GEngine->GetWorld(), 0, EQuitPreference::Quit, false);
#ifdef WITH_EDITOR
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, TEXT("Exit is disabled in the editor."));
#else
        FGenericPlatformMisc::RequestExit(false);
#endif
    });
}

bool Umain_menu::current_game_available()
{
    return false; // TODO: return true if there is a game currently being played; otherwise, check for a latest save-file
}
