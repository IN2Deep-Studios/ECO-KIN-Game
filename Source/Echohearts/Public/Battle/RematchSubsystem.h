// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RematchSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRematchTierUnlocked, int32, BossDexID, int32, NewTier);

// Boss Rematches: all 14 bosses, rematchable at rising resonance tiers
// with new phases. Victory unlocks the next tier — the old fights,
// remembered harder.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API URematchSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    void RegisterBoss(int32 BossDexID, int32 MaxTier);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    bool StartRematch(int32 BossDexID, int32 Tier);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    void ReportRematchVictory(int32 BossDexID);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Battle")
    int32 GetHighestTierCleared(int32 BossDexID) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Battle")
    FOnRematchTierUnlocked OnRematchTierUnlocked;

private:
    UPROPERTY()
    TMap<int32, int32> MaxTiers;

    UPROPERTY()
    TMap<int32, int32> HighestCleared;
};
