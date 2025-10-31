#include "Amain_menu_game_mode.h"
#include "Amain_menu_game_state.h"
#include "Amain_menu_hud.h"
#include "Amain_menu_controller.h"
#include "game_instance.h"

#include <DirectoryWatcherModule.h>
#include <Kismet/GameplayStatics.h>
#include <Modules/ModuleManager.h>

namespace {
    Amain_menu_game_state * cast(AGameStateBase * base)
    {
        return Cast<Amain_menu_game_state>(base);
    }

    TArray<FString> find_save_files()
    {
        FString dir = FPaths::ProjectSavedDir() + TEXT("SaveGames/");
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
    HUDClass = Amain_menu_hud::StaticClass();
    PlayerControllerClass = Amain_menu_controller::StaticClass();
    GameStateClass = Amain_menu_game_state::StaticClass();
}

void Amain_menu_game_mode::BeginPlay()
{
    Super::BeginPlay();

    TArray<FString> saves = find_save_files();
    cast(GameState)->saves_ = std::move(saves);
    cast(GameState)->saves_changed();

    FDirectoryWatcherModule & mod =
        FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(
            TEXT("DirectoryWatcher"));
    dir_watcher_ = mod.Get();
    if (dir_watcher_) {
        watched_dir_ = FPaths::ProjectSavedDir() + TEXT("SaveGames/");
        dir_watcher_->RegisterDirectoryChangedCallback_Handle(
            watched_dir_,
            IDirectoryWatcher::FDirectoryChanged::CreateUObject(
                this, &Amain_menu_game_mode::watched_dir_changed),
            dir_watcher_handle_,
            IDirectoryWatcher::WatchOptions::IgnoreChangesInSubtree);
    }
}

void Amain_menu_game_mode::EndPlay(EEndPlayReason::Type reason)
{
    Super::EndPlay(reason);

    if (!dir_watcher_ || !dir_watcher_handle_.IsValid())
        return;

    dir_watcher_->UnregisterDirectoryChangedCallback_Handle(
        watched_dir_, dir_watcher_handle_);
}

void Amain_menu_game_mode::multicast_new_game_Implementation()
{
    UGameplayStatics::OpenLevel(::world(), TEXT("playing"), true);
}

void Amain_menu_game_mode::watched_dir_changed(
    TArray<FFileChangeData> const & changes)
{
    for (FFileChangeData const & c : changes) {
        UE_LOG(LogTemp, Log, TEXT("File change detected: %s, Change Type: %d"),
               *c.Filename, (int)c.Action);
    }

    TArray<FString> saves = find_save_files();
    cast(GameState)->saves_ = std::move(saves);
    cast(GameState)->saves_changed();

    auto const it = std::find_if(
        begin(changes), end(changes),
        [](auto const & e) {
            return e.Action == FFileChangeData::FCA_RescanRequired;
        });
    if (it == end(changes)) {
        cast(GameState)->save_file_changes_.SetNum(changes.Num());
        std::transform(begin(changes), end(changes),
                       end(cast(GameState)->save_file_changes_),
                       [](auto & e) {
                           return Freplicable_FFileChangeData{
                               std::move(e.Filename), (uint8)e.Action};
                       });
        cast(GameState)->save_file_changes_changed();
    } else {
        cast(GameState)->save_file_changes_.SetNum(1);
        cast(GameState)->save_file_changes_[0] = Freplicable_FFileChangeData{
            std::move(it->Filename), (uint8)it->Action};
        cast(GameState)->save_file_changes_changed();
    }
}
