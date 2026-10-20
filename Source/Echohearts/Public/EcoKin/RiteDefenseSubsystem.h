// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EcoKin/RiteDefenseTypes.h"
#include "RiteDefenseSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDefenseStarted, FName, DefenseId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDefenseWave, FName, DefenseId, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDefenseCompleted, FName, DefenseId, bool, bCircleHeld);

// Rite Defense: protect the circle while a Growth Rite or purification
// completes. Orchestrates waves via delegates — the encounter spawner
// listens to OnDefenseWave and brings the wild; duel outcomes and soothe
// actions feed back through ReportIntruderResolved. Nothing dies here.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API URiteDefenseSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Rite")
    void RegisterDefense(const FRiteDefenseDef& Def);

    // Begins the defense. Returns false if unknown or already running.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Rite")
    bool BeginDefense(FName DefenseId);

    // Called when an intruder is resolved: true if soothed/befriended or
    // subdued-and-withdrawn, false if it breached the circle.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Rite")
    void ReportIntruderResolved(FName DefenseId, bool bKeptOut);

    // Ends the defense early (rite interrupted by the player).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Rite")
    void EndDefense(FName DefenseId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Rite")
    ERiteDefenseState GetDefenseState(FName DefenseId) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Rite")
    FOnDefenseStarted OnDefenseStarted;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Rite")
    FOnDefenseWave OnDefenseWave;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Rite")
    FOnDefenseCompleted OnDefenseCompleted;

private:
    void TickDefense(FName DefenseId);
    void FinishDefense(FName DefenseId, bool bCircleHeld);

    UPROPERTY()
    TMap<FName, FRiteDefenseDef> Defenses;

    UPROPERTY()
    TMap<FName, ERiteDefenseState> States;

    UPROPERTY()
    TMap<FName, float> ElapsedSeconds;

    UPROPERTY()
    TMap<FName, int32> NextWaveIndex;

    UPROPERTY()
    TMap<FName, int32> Breaches;

    FTimerHandle DefenseTimerHandle;
};
