#pragma once

#include "dir_watcher.h"

#include <array>
#include <filesystem>
#include <functional>
#include <vector>

#include <AssetRegistry/AssetRegistryModule.h>
#include <CoreMinimal.h>
#include <TimerManager.h>
#include <Engine/GameInstance.h>
#include <Engine/World.h>
#include <Internationalization/StringTableRegistry.h>
#include <Internationalization/StringTable.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Misc/Paths.h>
#include "game_instance.generated.h"


UENUM(BlueprintType)
enum class game_kind : uint8 { sp, mp };

UCLASS()
class SPACEPORT_API Ugame_instance : public UGameInstance
{
    GENERATED_BODY()

public:
    Ugame_instance() { self_ptr_ = this; }

    void
    watch_save_game_dir(std::function<void(std::vector<Ffile_change>)> callback)
    {
        check(callback);
        FString const dir = FPaths::ProjectSavedDir() + TEXT("SaveGames/");
        UE_LOG(LogTemp, Log, TEXT("Watching save dir %s for changes"), *dir);
        auto const & dir_chars = dir.GetCharArray();
        std::filesystem::path const dir_path(
            dir_chars.GetData(), dir_chars.GetData() + dir_chars.Num());

        unwatch_save_game_dir();
        dir_watcher_ = std::make_unique<dir_watcher>();
        dir_watcher_cb_ = callback;
        dir_watcher_->watch(dir_path);
        GetWorld()->GetTimerManager().SetTimer(
            dir_watch_updates_handle_,
            this,
            &Ugame_instance::dir_watch_update,
            0.5f,
            true);
    }
    void unwatch_save_game_dir()
    {
        if (dir_watch_updates_handle_.IsValid()) {
            GetWorld()->GetTimerManager().ClearTimer(dir_watch_updates_handle_);
        }
        dir_watcher_.reset();
        UE_LOG(LogTemp, Log, TEXT("No longer watching save dir for changes"));
    }

    ::game_kind game_kind() const { return game_kind_; }
    void game_kind(::game_kind kind) { game_kind_ = kind; }

    std::filesystem::path const & game_to_load() const { return game_to_load_; }
    void game_to_load(std::filesystem::path path)
    {
        game_to_load_ = std::move(path);
    }

    FText loc_text(FTextKey const & key)
    {
        if (string_table_id_.IsNone())
            string_table_id_ = string_table_->GetStringTableId();
        return FText::FromStringTable(string_table_id_, key);
    }

    static Ugame_instance * get() { return self_ptr_; }

private:
    UPROPERTY(
        EditAnywhere,
        Category = "Localization",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStringTable> string_table_;

    FName string_table_id_;
    ::game_kind game_kind_;
    std::filesystem::path game_to_load_;

    void dir_watch_update()
    {
        auto results = dir_watcher_->get_pending_results();
        if (!results.empty())
            dir_watcher_cb_(std::move(results));
    }
    std::unique_ptr<dir_watcher> dir_watcher_;
    FTimerHandle dir_watch_updates_handle_;
    std::function<void(std::vector<Ffile_change>)> dir_watcher_cb_;

    inline static Ugame_instance * self_ptr_ = nullptr;
};

inline UWorld * world() { return Ugame_instance::get()->GetWorld(); }

inline FText loc_text(FTextKey const & key)
{
    return Ugame_instance::get()->loc_text(key);
}

inline FText loc_text(std::string_view const & key)
{
    return Ugame_instance::get()->loc_text(
        FString::ConstructFromPtrSize(key.data(), key.size()));
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
