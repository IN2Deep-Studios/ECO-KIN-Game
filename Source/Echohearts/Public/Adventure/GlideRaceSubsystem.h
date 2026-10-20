// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlideRaceSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRaceCheckpoint, int32, RaceId, int32, Checkpoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRaceFinished, int32, RaceId, float, TimeSeconds, bool, bNewRecord);

// Glide Races: ride flying companions through sky-rings. Time trials,
// rival racers, and the eternal question of whether Maela Thorn can
// be beaten (she can; she talks too much before losing).
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UGlideRaceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    int32 StartRace(FName CourseId, int32 CheckpointCount);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportCheckpoint(int32 RaceId);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void FinishRace(int32 RaceId, float TimeSeconds);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Adventure")
    float GetBestTime(FName CourseId) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnRaceCheckpoint OnRaceCheckpoint;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnRaceFinished OnRaceFinished;

private:
    UPROPERTY()
    TMap<int32, FName> RaceCourses;

    UPROPERTY()
    TMap<int32, int32> RaceCheckpoints;

    UPROPERTY()
    TMap<FName, float> BestTimes;

    int32 NextRaceId = 1;
};
