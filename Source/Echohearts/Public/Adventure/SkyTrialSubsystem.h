// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SkyTrialSubsystem.generated.h"

// The five Sky Isles, each with its signature trial.
UENUM(BlueprintType)
enum class ESkyIsle : uint8
{
    GalewardSpires   UMETA(DisplayName = "Galeward Spires"),   // glide courses
    SunshardTerraces UMETA(DisplayName = "Sunshard Terraces"), // light puzzles
    MistveilGardens  UMETA(DisplayName = "Mistveil Gardens"),  // hidden gardens
    RiftAnchorage    UMETA(DisplayName = "Rift Anchorage"),    // shadow wards
    Stormcrown       UMETA(DisplayName = "Stormcrown")          // the tempest's eye
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTrialCompleted, ESkyIsle, Isle, bool, bFirstCompletion);

// Sky-Isle Trials: each isle's signature challenge. Complete them to
// open the isle's heart — and the Skyhold's deeper tiers.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API USkyTrialSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    bool StartTrial(ESkyIsle Isle);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportTrialComplete(ESkyIsle Isle);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Adventure")
    bool IsTrialComplete(ESkyIsle Isle) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Adventure")
    int32 GetTrialsCompleted() const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnTrialCompleted OnTrialCompleted;

private:
    UPROPERTY()
    TSet<ESkyIsle> CompletedTrials;

    UPROPERTY()
    TSet<ESkyIsle> ActiveTrials;
};
