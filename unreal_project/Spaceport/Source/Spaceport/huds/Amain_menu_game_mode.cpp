#include "Amain_menu_game_mode.h"
#include "Amain_menu_game_state.h"
#include "Amain_menu_hud.h"
#include "Amain_menu_controller.h"
#include "game_instance.h"


namespace {
    Amain_menu_game_state * cast(AGameStateBase * base)
    {
        return Cast<Amain_menu_game_state>(base);
    }

    TArray<FString> find_save_files()
    {
        FString dir = FPaths::ProjectSavedDir() + TEXT("SaveGames/");
        UE_LOG(LogTemp, Warning, TEXT("Searching saves dir %s for files"), *dir);
        TArray<FString> saves;
        IFileManager::Get().FindFiles(saves, *dir, TEXT("*.sav"));
        return std::move(saves);
    }

    // TODO: Move to some utility header.
    template<typename T>
    T * begin(TArray<T> & a) { return a.GetData(); }
    template<typename T>
    T * end(TArray<T> & a) { return a.GetData() + a.Num(); }
    template<typename T>
    T const * begin(TArray<T> const & a) { return a.GetData(); }
    template<typename T>
    T const * end(TArray<T> const & a) { return a.GetData() + a.Num(); }
}

Amain_menu_game_mode::Amain_menu_game_mode(FObjectInitializer const & init) :
    AGameModeBase(init)
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
    UE_LOG(LogTemp, Log, TEXT("ENTER Amain_menu_game_mode::BeginPlay()"));
    Super::BeginPlay();

    TArray<FString> saves = find_save_files();
    cast(GameState)->saves_ = std::move(saves);
    cast(GameState)->saves_changed();

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

void Amain_menu_game_mode::multicast_new_game_Implementation()
{
    Ugame_instance::get()->load_playing_level();
}

void Amain_menu_game_mode::saves_dir_changed(
    std::vector<Ffile_change> changes)
{
    for (auto const & c : changes) {
        UE_LOG(LogTemp, Log, TEXT("File change detected: %s, Change Type: %d"),
               *c.file, (int)c.kind);
    }

    auto const it = std::ranges::find_if(
        changes,
        [](auto const & e) {
            return e.kind == Efile_change_kind::rescan_required;
        });
    if (it == changes.end()) {
        cast(GameState)->save_file_changes_.SetNum(changes.size());
        std::ranges::move(changes, begin(cast(GameState)->save_file_changes_));
        cast(GameState)->save_file_changes_changed();
    } else {
        TArray<FString> saves = find_save_files();
        cast(GameState)->saves_ = std::move(saves);
        cast(GameState)->saves_changed();
    }
}
