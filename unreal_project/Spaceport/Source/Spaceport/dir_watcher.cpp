#include "dir_watcher.h"

#include <logging.hpp>


namespace {
    constexpr auto watch_flags =
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE |
        FILE_NOTIFY_CHANGE_CREATION;
    // win32 platform max buffer bytes is 64k
    constexpr int max_buf_size = 64 * 1024;
}

dir_watcher::dir_watcher() : buf_(max_buf_size), buf2_(buf_.size())
{
    overlapped_ = {.hEvent = this};
}

dir_watcher::~dir_watcher()
{
    reset();
}

bool dir_watcher::watch(std::filesystem::path const & dir)
{
    if (!dir_.empty())
        return false;

    dir_ = dir;
    auto const abs_path = std::filesystem::absolute(dir_);
    handle_ = CreateFile(
        abs_path.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        nullptr);

    if (handle_ == INVALID_HANDLE_VALUE) {
        DWORD const err = GetLastError();
        UE_LOG(
            LogTemp, // TODO
            Display,
            TEXT("Call to CreateFile failed for '%s' (code: [%d])."),
            reinterpret_cast<TCHAR const *>(abs_path.c_str()),
            err);
        return false;
    }

    bool const success = !!ReadDirectoryChangesW(
        handle_,
        buf_.data(),
        buf_.size() * sizeof(buf_[0]),
        false,
        watch_flags,
        NULL,
        &overlapped_,
        &w32_callback);

    if (!success) {
        DWORD const err = GetLastError();
        UE_LOG(
            LogTemp,
            Display,
            TEXT("Initial call to ReadDirectoryChangesW failed for '%s' (code: "
                 "[%d])."),
            reinterpret_cast<TCHAR const *>(dir_.c_str()),
            err);
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
        return false;
    }

    buf_dirty_ = true;

    return true;
}

std::vector<Ffile_change> dir_watcher::get_pending_results()
{
    SleepEx(0, 1); // Flush any pendings from the win32 message queue.
    return std::move(changes_);
}

void dir_watcher::reset()
{
    if (!reset_called_ && !done_ && handle_ != INVALID_HANDLE_VALUE) {
        CancelIoEx(handle_, &overlapped_);
        HANDLE h = handle_;
        handle_ = INVALID_HANDLE_VALUE;
        WaitForSingleObjectEx(h, 1000, true);
        CloseHandle(h);
        buf_dirty_ = false;
    }
    reset_called_ = true;
    done_ = true;
}

void dir_watcher::process_changes(unsigned int err, unsigned int bytes)
{
    buf_dirty_ = false;

    auto shut_down = [this]() {
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
        done_ = true;
    };

    if (err == 0 && bytes == 0) {
        DWORD dummy;
        GetOverlappedResult(handle_, &overlapped_, &dummy, 0);
        err = GetLastError();
    }

    if (err == ERROR_OPERATION_ABORTED) {
        done_ = true;
        UE_CLOG(
            !IsEngineExitRequested(),
            LogTemp,
            Log,
            TEXT("Directory-watch for '%s' aborted."),
            reinterpret_cast<TCHAR const *>(dir_.c_str()));
        return;
    }

    bool const changes_valid = err != ERROR_IO_INCOMPLETE && 0 < bytes;
    bool need_rescan = false;
    if (changes_valid) {
        std::swap(buf_, buf2_);
    } else if (err == ERROR_INVALID_PARAMETER) {
        UE_LOG(
            LogTemp,
            Display,
            TEXT("Directory-watch for '%s' failed (code: "
                 "ERROR_INVALID_PARAMETER); trying again."),
            reinterpret_cast<TCHAR const *>(dir_.c_str()));
    } else if (err == ERROR_ACCESS_DENIED) {
        shut_down();
        UE_LOG(
            LogTemp,
            Display,
            TEXT("Directory-watch for '%s' failed, due to permissions; "
                 "aborting."),
            reinterpret_cast<TCHAR const *>(dir_.c_str()));
        return;
    } else if (err == ERROR_NOTIFY_ENUM_DIR) {
        need_rescan = true;
    } else if (err != ERROR_SUCCESS) {
        UE_LOG(
            LogTemp,
            Display,
            TEXT("Directory-watch for '%s' failed (code: [%d]). Trying again."),
            reinterpret_cast<TCHAR const *>(dir_.c_str()),
            err);
    } else {
        UE_LOG(
            LogTemp,
            Display,
            TEXT("Directory-watch for '%s' failed, possibly due to buffer "
                 "overflow; trying again."),
            reinterpret_cast<TCHAR const *>(dir_.c_str()));
    }

    // Start up another read
    bool const success = !!ReadDirectoryChangesW(
        handle_,
        buf_.data(),
        buf_.size() * sizeof(buf_[0]),
        false,
        watch_flags,
        nullptr,
        &overlapped_,
        &w32_callback);

    if (!success) {
        DWORD const err = GetLastError();
        UE_LOG(
            LogTemp,
            Display,
            TEXT("Call to ReadDirectoryChangesW for '%s' failed (code: [%d], "
                 "handle: [%p]); trying again."),
            reinterpret_cast<TCHAR const *>(dir_.c_str()),
            err,
            handle_);
        shut_down();
        return;
    }
    buf_dirty_ = true;

    if (need_rescan) {
        // When a rescan is required, we don't need any of the other changes.
        changes_.clear();
        changes_.push_back(
            Ffile_change{FString(dir_.c_str()),
                Efile_change_kind::rescan_required});
        return;
    }

    if (!changes_valid)
        return;

    std::byte * byte_ptr = buf2_.data();
    while (1) {
        auto * const info =
            reinterpret_cast<FILE_NOTIFY_INFORMATION *>(byte_ptr);

        std::wstring name;
        static_assert(sizeof(WCHAR) == sizeof(TCHAR));
        int const size = info->FileNameLength / sizeof(WCHAR);
        name = std::wstring(info->FileName, info->FileNameLength);

        Efile_change_kind change_kind = {};
        switch (info->Action) {
        case FILE_ACTION_ADDED:
        case FILE_ACTION_RENAMED_NEW_NAME:
            change_kind = Efile_change_kind::added;
            break;
        case FILE_ACTION_REMOVED:
        case FILE_ACTION_RENAMED_OLD_NAME:
            change_kind = Efile_change_kind::removed;
            break;
        case FILE_ACTION_MODIFIED:
            change_kind = Efile_change_kind::modified;
            break;
        default: change_kind = Efile_change_kind::unknown; break;
        }

        if (change_kind != Efile_change_kind::unknown) {
            auto const path = dir_ / std::filesystem::path(std::move(name));
            changes_.push_back(
                Ffile_change{FString(path.c_str()), change_kind});
        }

        if (info->NextEntryOffset == 0)
            break;

        byte_ptr += info->NextEntryOffset;
    }
}
