#include "Agame_mode_base.h"
#include "Agame_state.h"
#include "Aplayer_state.h"
#include "game_instance.h"
#include "utility.hpp"


namespace {
    Agame_state* cast(AGameStateBase * base)
    {
        return Cast<Agame_state>(base);
    }
}

Agame_mode_base::Agame_mode_base(FObjectInitializer const & init) :
    AGameModeBase(init)
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode_base CTOR"));
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode_base CTOR"));
}

void Agame_mode_base::PostLogin(APlayerController * player)
{
    Super::PostLogin(player);

    UE_LOG(LogTemp, Log, TEXT("Player logged in."));

    if (!player)
        return;

    auto * ps = Cast<Aplayer_state>(player->PlayerState);
    if (!ps)
        return;

    ps->player_id(players_);
    ++players_;
}

void Agame_mode_base::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode_base::BeginPlay()"));
    publish_save_files();
    Ugame_instance::get()->watch_save_game_dir([this](auto changes) {
        saves_dir_changed(std::move(changes));
    });
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode_base::BeginPlay()"));
}

void Agame_mode_base::EndPlay(EEndPlayReason::Type reason)
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode_base::EndPlay()"));
    Super::EndPlay(reason);

    Ugame_instance::get()->unwatch_save_game_dir();
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode_base::EndPlay()"));
}

void Agame_mode_base::multicast_load_playing_Implementation()
{
    Ugame_instance::get()->load(level::playing);
}

void Agame_mode_base::load_and_start_newest_game_Implementation()
{
    TArray<FString> const saves = find_save_files(true);
    std::filesystem::path newest;

    if (!saves.IsEmpty()) {
        std::filesystem::path const dir = save_dir_path();
        newest = dir / to_path(saves[0]);
        std::filesystem::file_time_type newest_t = last_write_time(newest);
        for (auto const & save : saves) {
            std::filesystem::path p = dir / to_path(save);
            auto const t = last_write_time(p);
            if (newest_t < t) {
                newest = std::move(p);
                newest_t = t;
            }
        }
    }

    if (newest.empty()) {
        if (auto * hud = hud_base()) {
            hud->notify_user(
                TEXT("load_game_failed"),
                TEXT("continue_failed_no_saves"));
        }
    } else {
        Ugame_instance::get()->game_to_load(newest);
        multicast_load_playing();
    }
}

void Agame_mode_base::load_and_start_game_Implementation(
    FString const & filename)
{
    auto f = to_path(filename);
    f += TEXT(".sav");
    Ugame_instance::get()->game_to_load(save_dir_path() / f);
    multicast_load_playing();
}

void Agame_mode_base::publish_save_files()
{
    TArray<FString> saves = find_save_files();

    // Filter out known non-game .sav files.
    auto const first = begin(saves);
    auto const last = end(saves);
    auto it = std::remove_if(first, last, [](auto const & e) {
        return e == TEXT("EnhancedInputUserSettings");
    });
    if (it != last)
        saves.SetNum(saves.Num() - (last - it));

    UE_LOG(
        LogTemp,
        Log,
        TEXT("Publish save files: %s"),
        *FString::Join(saves, TEXT(", ")));
    cast(GameState)->saves_ = std::move(saves);
    cast(GameState)->saves_changed();
}

void Agame_mode_base::saves_dir_changed(
    std::vector<Ffile_change> changes)
{
    for (auto const & c : changes) {
        UE_LOG(LogTemp, Log, TEXT("File change detected: %s, Change Type: %d"),
               *c.file, (int)c.kind);
    }

    publish_save_files();
    cast(GameState)->save_file_changes_.SetNum(changes.size());
    std::ranges::move(changes, begin(cast(GameState)->save_file_changes_));
    cast(GameState)->save_file_changes_changed();
}
