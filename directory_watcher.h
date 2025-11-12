#pragma once

#include <CoreMinimal.h>
#include <Kismet/BlueprintFunctionLibrary.h>
#include <Async/Async.h>
#include "Udir_watcher.generated.h"


DECLARE_DYNAMIC_DELEGATE(FOnFileChangedDelegate);

struct FWindowsFileChangeWatcher;

UCLASS()
class Udir_watcher_lib: UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    Udir_watcher_lib

    UFUNCTION(BlueprintCallable, Category = "File Watching")
    static void watch(
        FString const & dir,
        FOnFileChangedDelegate const & delegate);

    UFUNCTION(BlueprintCallable, Category = "File Watching")
    static void unwatch();

private:
    static std::unique_ptr<dir_watcher_impl> impl_;
};
