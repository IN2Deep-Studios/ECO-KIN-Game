// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AbyssDiveSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDiveDepth, int32, DiveId, float, DepthMeters);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbyssDiscovery, int32, DiveId, FName, DiscoveryId);

// Abyss Dives: Hadalon's territory. Deep-water exploration where
// pressure is the mechanic and bioluminescence is the reward. The
// trenches remember everything.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UAbyssDiveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    int32 StartDive();

    // Depth in meters; pressure strain scales with it (the Anima-Link
    // carries it — diving is a team effort).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportDepth(int32 DiveId, float DepthMeters);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportDiscovery(int32 DiveId, FName DiscoveryId);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void EndDive(int32 DiveId, bool bSurfaced);

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnDiveDepth OnDiveDepth;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnAbyssDiscovery OnAbyssDiscovery;

private:
    UPROPERTY()
    TSet<int32> ActiveDives;

    int32 NextDiveId = 1;
};
