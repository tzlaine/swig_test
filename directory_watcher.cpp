#include "MyCustomFileWatchingFunctionLibrary.h"
#include <mutex>
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"


struct FWindowsFileChangeWatcher final : FRunnable
{
    HANDLE DirectoryHandle;
    HANDLE ChangeHandle;
    FString DirectoryToWatch;
    bool bShouldWatch;
    FRunnableThread * WatchThread;
    FOnFileChangedDelegate OnChangeDelegate;

    FWindowsFileChangeWatcher() :
        DirectoryHandle(INVALID_HANDLE_VALUE),
        ChangeHandle(INVALID_HANDLE_VALUE),
        bShouldWatch(false),
        WatchThread(nullptr)
    {}

    virtual uint32 Run() override
    {
        while (bShouldWatch) {
            switch (WaitForSingleObject(ChangeHandle, INFINITE)) {
            case WAIT_OBJECT_0:
                UE_LOG(LogTemp, Log, TEXT("Directory change detected."));
                if (OnChangeDelegate.IsBound()) {
                    AsyncTask(ENamedThreads::GameThread, [this]() {
                        OnChangeDelegate.ExecuteIfBound();
                    });
                }
                if (FindNextChangeNotification(ChangeHandle) ==
                    Windows::FALSE) {
                    UE_LOG(
                        LogTemp,
                        Error,
                        TEXT("FindNextChangeNotification failed."));
                    bShouldWatch = false;
                }
                break;

            case WAIT_FAILED:
                UE_LOG(LogTemp, Error, TEXT("WaitForSingleObject failed."));
                bShouldWatch = false;
                break;
            }
        }
        return 0;
    }

    void StartWatching()
    {
        bShouldWatch = true;
        WatchThread =
            FRunnableThread::Create(this, TEXT("FileChangeWatcherThread"));
    }

    void StopWatching()
    {
        bShouldWatch = false;

        if (WatchThread) {
            WatchThread->Kill(true);
            delete WatchThread;
            WatchThread = nullptr;
        }

        if (ChangeHandle != INVALID_HANDLE_VALUE) {
            FindCloseChangeNotification(ChangeHandle);
            ChangeHandle = INVALID_HANDLE_VALUE;
        }

        if (DirectoryHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(DirectoryHandle);
            DirectoryHandle = INVALID_HANDLE_VALUE;
        }
    }
};

FWindowsFileChangeWatcher *
    UMyCustomFileWatchingFunctionLibrary::FileChangeWatcher = nullptr;

void UMyCustomFileWatchingFunctionLibrary::StartWatching(
    const FString & DirectoryToWatch,
    const FOnFileChangedDelegate & OnChangeDelegate)
{
    if (FileChangeWatcher && FileChangeWatcher->bShouldWatch) {
        UE_LOG(LogTemp, Warning, TEXT("Already watching a directory."));
        return;
    }

    if (!FileChangeWatcher) {
        FileChangeWatcher = new FWindowsFileChangeWatcher();
    }

    FileChangeWatcher->DirectoryToWatch = DirectoryToWatch;
    FileChangeWatcher->OnChangeDelegate = OnChangeDelegate;

    FileChangeWatcher->DirectoryHandle = CreateFile(
        *FileChangeWatcher->DirectoryToWatch,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr);

    if (FileChangeWatcher->DirectoryHandle == INVALID_HANDLE_VALUE) {
        UE_LOG(LogTemp, Error, TEXT("Failed to get directory handle."));
        return;
    }

    FileChangeWatcher->ChangeHandle = FindFirstChangeNotification(
        *FileChangeWatcher->DirectoryToWatch,
        Windows::FALSE,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE);

    if (FileChangeWatcher->ChangeHandle == INVALID_HANDLE_VALUE) {
        UE_LOG(LogTemp, Error, TEXT("Failed to create change notification."));
        CloseHandle(FileChangeWatcher->DirectoryHandle);
        return;
    }

    FileChangeWatcher->StartWatching();
}

void UMyCustomFileWatchingFunctionLibrary::StopWatching()
{
    if (!FileChangeWatcher || !FileChangeWatcher->bShouldWatch) {
        return;
    }

    FileChangeWatcher->StopWatching();

    delete FileChangeWatcher;
    FileChangeWatcher = nullptr;
}
