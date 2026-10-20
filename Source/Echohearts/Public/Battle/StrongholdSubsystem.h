// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StrongholdSubsystem.generated.h"

UENUM(BlueprintType)
enum class EAssaultApproach : uint8
{
    Stealth UMETA(DisplayName = "Stealth"), // Shimmer-veiled, quiet, precise
    Loud    UMETA(DisplayName = "Loud")     // Battery crews and war-cries
};

UENUM(BlueprintType)
enum class EAssaultState : uint8
{
    Quiet     UMETA(DisplayName = "Quiet"),
    Underway  UMETA(DisplayName = "Underway"),
    Breached  UMETA(DisplayName = "Breached"),  // inner ward down
    Completed UMETA(DisplayName = "Completed"),
    Repelled  UMETA(DisplayName = "Repelled")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAssaultStarted, FName, StrongholdId, EAssaultApproach, Approach);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAssaultEnded, FName, StrongholdId, bool, bVictory);

// Society Stronghold Assaults: we defended the Sanctuary — now we take
// the fight to them. Stealth (Shimmer-veiled infiltration, quiet
// objectives) or Loud (ward-breaking assault). Objectives fall one by
// one; the vault at the heart holds what they stole.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UStrongholdSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void RegisterStronghold(FName StrongholdId, const FText& Title, int32 ObjectiveCount);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    bool StartAssault(FName StrongholdId, EAssaultApproach Approach);

    // One objective down (ward pylon, gate, lieutenant...). At all
    // objectives, the stronghold falls.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportObjectiveComplete(FName StrongholdId);

    // Driven off: the assault fails, the societies reinforce (+1
    // objective next attempt — they learn).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportRepelled(FName StrongholdId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Adventure")
    EAssaultState GetAssaultState(FName StrongholdId) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnAssaultStarted OnAssaultStarted;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnAssaultEnded OnAssaultEnded;

private:
    UPROPERTY()
    TMap<FName, int32> ObjectiveTotals;

    UPROPERTY()
    TMap<FName, int32> ObjectivesDone;

    UPROPERTY()
    TMap<FName, EAssaultState> States;
};
