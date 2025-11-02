#pragma once

#include <filesystem>
#include <vector>

#if defined(_MSC_VER)
#include <Windows/WindowsHWrapper.h>
#endif

#include "dir_watcher.generated.h"


UENUM(BlueprintType)
enum struct Efile_change_kind : uint8 {
    unknown,
    added,
    modified,
    removed,
    rescan_required,
};

USTRUCT(BlueprintType)
struct Ffile_change
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString file;

    UPROPERTY(BlueprintReadOnly)
    Efile_change_kind kind;

    bool operator==(Ffile_change const &) const = default;
};

struct dir_watcher
{
    dir_watcher();
    ~dir_watcher();

    bool watch(std::filesystem::path const & dir);
    std::vector<Ffile_change> get_pending_results();
    void reset();

private:
    void process_changes(unsigned int err, unsigned int bytes);

    static void w32_callback(DWORD err, DWORD bytes, LPOVERLAPPED overlapped)
    {
        auto * this_ = reinterpret_cast<dir_watcher *>(overlapped->hEvent);
        this_->process_changes((unsigned int)err, (unsigned int)bytes);
    }

#if defined(_MSC_VER)
    std::filesystem::path dir_;
    std::vector<std::byte> buf_;
    std::vector<std::byte> buf2_;
    OVERLAPPED overlapped_;
    std::vector<Ffile_change> changes_;
    HANDLE handle_ = INVALID_HANDLE_VALUE;
    bool buf_dirty_ = true;
    bool done_ = false;
    bool reset_called_ = false;
#endif
};
