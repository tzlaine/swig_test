#pragma once

#include <CoreMinimal.h>
#include <Sound/SoundBase.h>
#include <Sound/SoundClass.h>
#include <Sound/SoundMix.h>
#include "audio_assets.generated.h"


UCLASS(BlueprintType, Blueprintable)
class Uaudio_assets_t : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Audio cues")
    TObjectPtr<USoundBase> click_cue_;
    UPROPERTY(EditAnywhere, Category = "Audio cues")
    TObjectPtr<USoundBase> yes_cue_;
    UPROPERTY(EditAnywhere, Category = "Audio cues")
    TObjectPtr<USoundBase> no_cue_;

    UPROPERTY(EditAnywhere, Category = "Sound classes")
    TMap<FString, TObjectPtr<USoundClass>> sound_classes_;
    UPROPERTY(EditAnywhere, Category = "Sound classes")
    TMap<FString, TObjectPtr<USoundMix>> sound_mixes_;
};

bool have_audio_assets();

Uaudio_assets_t const & audio_assets();
