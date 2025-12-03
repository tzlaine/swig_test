#pragma once

#include <CoreMinimal.h>
#include <Sound/SoundBase.h>
#include <Sound/SoundClass.h>
#include <Sound/SoundMix.h>
#include "audio_cues.generated.h"


UCLASS(BlueprintType, Blueprintable)
class Uaudio_cues_t : public UObject
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

bool have_audio_cues();

Uaudio_cues_t const & audio_cues();
