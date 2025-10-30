#pragma once

#include <cassert>

#include <AssetRegistry/AssetRegistryModule.h>
#include <CoreMinimal.h>
#include <Engine/GameInstance.h>
#include <Internationalization/StringTableRegistry.h>
#include <Internationalization/StringTable.h>
#include <Kismet/KismetSystemLibrary.h>
#include "game_instance.generated.h"


UCLASS()
class SPACEPORT_API Ugame_instance : public UGameInstance
{
    GENERATED_BODY()
	
public:
    Ugame_instance()
    {
        assert(!self_ptr_);
        self_ptr_ = this;
    }

    UPROPERTY(EditAnywhere, Category = "Localization")
    TSoftObjectPtr<UStringTable> string_table_;

    FText loc_text(FTextKey const & key)
    {
        if (string_table_id_.IsNone()) {
            if (!string_table_.IsValid()) {
                string_table_ = TSoftObjectPtr<UStringTable>(
                    FString(TEXT("/Game/strings.strings")));
            }
            string_table_id_ =
                string_table_.LoadSynchronous()->GetStringTableId();
        }
        return FText::FromStringTable(string_table_id_, key);
    }

    static Ugame_instance * get()
    {
        return self_ptr_;
    }

private:
    FName string_table_id_;

    inline static Ugame_instance * self_ptr_ = nullptr;
};

inline FText loc_text(FTextKey const & key)
{
    return Ugame_instance::get()->loc_text(key);
}

inline void quit_game()
{
    auto const world = Ugame_instance::get()->GetWorld();
    for (FConstPlayerControllerIterator it =
             world->GetPlayerControllerIterator();
         it;
         ++it) {
        auto const controller = it->Get();
        UKismetSystemLibrary::QuitGame(
            world, controller, EQuitPreference::Quit, true);
    }
}
