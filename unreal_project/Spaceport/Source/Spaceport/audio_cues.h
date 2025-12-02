#pragma once

#include <CoreMinimal.h>
#include <Sound/SoundBase.h>
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
};

Uaudio_cues_t const & audio_cues();
