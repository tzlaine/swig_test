#include "Amain_menu_game_mode.h"
#include "Amain_menu_game_state.h"
#include "Amain_menu_hud.h"
#include "Amain_menu_controller.h"
#include "game_instance.h"
#include "utility.hpp"


namespace {
    Amain_menu_game_state * cast(AGameStateBase * base)
    {
        return Cast<Amain_menu_game_state>(base);
    }
}

Amain_menu_game_mode::Amain_menu_game_mode(FObjectInitializer const & init) :
    Agame_mode_base(init)
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Amain_menu_game_mode CTOR"));
    HUDClass = Amain_menu_hud::StaticClass();
    PlayerControllerClass = Amain_menu_controller::StaticClass();
    GameStateClass = Amain_menu_game_state::StaticClass();

    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = true;
    UE_LOG(LogTemp, Log, TEXT("EXIT Amain_menu_game_mode CTOR"));
}

void Amain_menu_game_mode::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Amain_menu_game_mode::BeginPlay()"));
    publish_save_files();
    Ugame_instance::get()->watch_save_game_dir([this](auto changes) {
        saves_dir_changed(std::move(changes));
    });
    UE_LOG(LogTemp, Log, TEXT("EXIT Amain_menu_game_mode::BeginPlay()"));
}

void Amain_menu_game_mode::EndPlay(EEndPlayReason::Type reason)
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Amain_menu_game_mode::EndPlay()"));
    Super::EndPlay(reason);

    Ugame_instance::get()->unwatch_save_game_dir();
    UE_LOG(LogTemp, Log, TEXT("EXIT Amain_menu_game_mode::EndPlay()"));
}

void Amain_menu_game_mode::load_and_start_newest_game_Implementation()
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
            if (t < newest_t) {
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

void Amain_menu_game_mode::saves_dir_changed(
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
