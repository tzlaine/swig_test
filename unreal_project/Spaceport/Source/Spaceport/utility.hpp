#pragma once

inline constexpr float meters = 100.0f;

template <typename T>
void call_real_soon(FTimerHandle& timer, T* t_ptr, void (T::*member_ptr)())
{
    t_ptr->GetWorldTimerManager().SetTimer(timer, t_ptr, member_ptr, 0.001, false);
}

inline TArray<FString> find_save_files()
{
    FString dir = FPaths::ProjectSavedDir() + TEXT("SaveGames/");
    TArray<FString> saves;
    IFileManager::Get().FindFiles(saves, *dir, TEXT("*.sav"));
    return std::move(saves);
}

inline bool have_any_save_files()
{
    return !find_save_files().IsEmpty();
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
