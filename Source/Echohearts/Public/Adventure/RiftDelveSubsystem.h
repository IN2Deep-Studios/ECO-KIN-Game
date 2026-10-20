// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RiftDelveSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDelveDepth, int32, DelveId, int32, Depth);

// Rift Delves: descents into the Rift Anchorage's Shade-touched depths,
// where Vaeruun's shadow lingers. Light is the mechanic — ward strength
// vs. depth. The deeper the dark, the brighter you must be.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API URiftDelveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    int32 StartDelve();

    // Descend one level; returns false if the ward can't hold the dark.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    bool DescendLevel(int32 DelveId, float WardStrength);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void EndDelve(int32 DelveId, bool bWithdrew);

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnDelveDepth OnDelveDepth;

private:
    UPROPERTY()
    TMap<int32, int32> DelveDepths;

    int32 NextDelveId = 1;
};
