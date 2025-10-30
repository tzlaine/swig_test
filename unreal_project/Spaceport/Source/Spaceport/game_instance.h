#pragma once

#include <filesystem>
#include <cassert>

#include <AssetRegistry/AssetRegistryModule.h>
#include <CoreMinimal.h>
#include <Engine/GameInstance.h>
#include <Engine/World.h>
#include <Internationalization/StringTableRegistry.h>
#include <Internationalization/StringTable.h>
#include <Kismet/KismetSystemLibrary.h>
#include "game_instance.generated.h"


UENUM(BlueprintType)
enum class game_kind : uint8 {
    sp,
    mp
};

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

    ::game_kind game_kind() const
    {
        return game_kind_;
    }
    void game_kind(::game_kind kind)
    {
        game_kind_ = kind;
    }

    std::filesystem::path const & game_to_load() const
    {
        return game_to_load_;
    }
    void game_to_load(FFilePath const & path)
    {
#if _MSC_VER
        std::wstring s(*path.FilePath);
#else
        std::string s(TCHAR_TO_UTF8(*path.FilePath));
#endif
        game_to_load_ = std::filesystem::path(std::move(s));
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
    ::game_kind game_kind_;
    std::filesystem::path game_to_load_;

    inline static Ugame_instance * self_ptr_ = nullptr;
};

inline UWorld * world()
{
    return Ugame_instance::get()->GetWorld();
}

inline FText loc_text(FTextKey const & key)
{
    return Ugame_instance::get()->loc_text(key);
}

inline void quit_game()
{
    auto const world = ::world();
    for (FConstPlayerControllerIterator it =
             world->GetPlayerControllerIterator();
         it;
         ++it) {
        auto const controller = it->Get();
        UKismetSystemLibrary::QuitGame(
            world, controller, EQuitPreference::Quit, true);
    }
}
