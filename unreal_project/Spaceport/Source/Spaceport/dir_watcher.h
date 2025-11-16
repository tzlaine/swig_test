#pragma once

#include <filesystem>
#include <vector>

#if defined(_MSC_VER)

#if defined(BUILD_FOR_TEST)
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include <Windows.h>
#if defined(min)
#undef min
#endif
#if defined(max)
#undef max
#endif
#define UENUM(_)
#define USTRUCT(_)
#define GENERATED_BODY()
#define UPROPERTY(_)
#define uint8 unsigned char
#define FString std::wstring
#else
#include <Windows/WindowsHWrapper.h>
#include "dir_watcher.generated.h"
#endif

#else

#error "dir_watcher has no implementation for this platform."

#endif


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
#if defined(_MSC_VER)
    void process_changes(unsigned int err, unsigned int bytes);

    static void w32_callback(DWORD err, DWORD bytes, LPOVERLAPPED overlapped)
    {
        auto * this_ = reinterpret_cast<dir_watcher *>(overlapped->hEvent);
        this_->process_changes((unsigned int)err, (unsigned int)bytes);
    }

    std::filesystem::path dir_;
    std::vector<std::byte> buf_;
    std::vector<std::byte> buf2_;
    OVERLAPPED overlapped_;
    std::vector<Ffile_change> changes_;
    HANDLE handle_ = INVALID_HANDLE_VALUE;
    bool buf_dirty_ = true;
    bool done_ = false;
    bool reset_called_ = false;
#else
    // TODO
#endif
};
