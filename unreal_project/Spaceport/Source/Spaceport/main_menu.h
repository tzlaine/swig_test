// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "game_state_actor.h"
#include "widgets/styled_button.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "main_menu.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Umain_menu : public UUserWidget
{
	GENERATED_BODY()

    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    Ustyled_button * continue_bn = nullptr;
    UPROPERTY(meta = (BindWidget))
    Ustyled_button * new_game_bn = nullptr;
    UPROPERTY(meta = (BindWidget))
    Ustyled_button * options_bn = nullptr;
    UPROPERTY(meta = (BindWidget))
    Ustyled_button * exit_bn = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actors")
    TSoftObjectPtr<Agame_state_actor> game_state_actor;

private:
    bool current_game_available();

};
