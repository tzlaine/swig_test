#pragma once

#include <CoreMinimal.h>
#include <GameFramework/GameUserSettings.h>
#include "game_user_settings.generated.h"


class Aplayer_controller_base;

UCLASS(config=GameUserSettings)
class Ugame_user_settings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
    Ugame_user_settings(FObjectInitializer const & init);

    void apply_console_settings(Aplayer_controller_base & pc);
    void apply_volume_settings();

    void LoadSettings(bool b = false) override;
    void ApplySettings(bool b) override;

    UPROPERTY(config, BlueprintReadWrite, Category = "Video")
    int texture_quality;

    UPROPERTY(config, BlueprintReadWrite, Category = "Video")
    int aa_method;

    UPROPERTY(config, BlueprintReadWrite, Category = "Video")
    int aa_quality;

    UPROPERTY(config, BlueprintReadWrite, Category = "Video")
    int planet_detail;

    UPROPERTY(config, BlueprintReadWrite, Category = "Audio")
    TMap<FString, float> volume_levels;

    static Ugame_user_settings * get();

private:
    void apply_impl();
    void apply_volume_settings_impl();

    bool unapplied_volume_settings_ = false;
};
