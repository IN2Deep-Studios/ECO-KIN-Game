// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "EchoheartsAudioManager.generated.h"

// What the music is saying. The score follows the bond and the world —
// combat strains it, restoration heals it, ceremonies lift it.
UENUM(BlueprintType)
enum class EMusicMood : uint8
{
    Exploration  UMETA(DisplayName = "Exploration"),
    Tension      UMETA(DisplayName = "Tension"),
    Combat       UMETA(DisplayName = "Combat"),
    Ceremony     UMETA(DisplayName = "Ceremony"),
    Restoration  UMETA(DisplayName = "Restoration"),
    Sorrow       UMETA(DisplayName = "Sorrow")
};

// Central audio routing: music moods, essence stingers, UI and world SFX.
// Assets are assigned in Blueprint/data; this manager owns WHEN things play.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API UEchoheartsAudioManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Audio")
    void SetMusicMood(EMusicMood NewMood);

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Audio")
    EMusicMood CurrentMood = EMusicMood::Exploration;

    // Short musical signature when an essence ability lands.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Audio")
    void PlayEssenceStinger(EEcoKinEssence Essence);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Audio")
    void PlayUIConfirm();

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Audio")
    void PlayUICancel();

    // The Binding ceremony has its own rising tone — a relationship beginning
    // should sound like one.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Audio")
    void PlayBindingChime();

    // Growth Rite completion: the Chorus.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Audio")
    void PlayRiteChorus();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMusicMoodChanged, EMusicMood, NewMood);

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Audio")
    FOnMusicMoodChanged OnMusicMoodChanged;
};
