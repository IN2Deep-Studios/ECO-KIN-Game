// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EscortSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEscortProgress, FName, EscortId, float, ProgressPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEscortEnded, FName, EscortId, bool, bArrived);

// Migration Escorts: guide EcoKin herds through dangerous territory.
// Moving defense — threats come to the herd, you turn them aside.
// Nothing is killed; everything arrives.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UEscortSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    bool StartEscort(FName EscortId, const FText& HerdName);

    // A threat repelled along the route (driven off, never killed).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportThreatRepelled(FName EscortId);

    // The herd advances; progress 0-100.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportProgress(FName EscortId, float ProgressPercent);

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnEscortProgress OnEscortProgress;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnEscortEnded OnEscortEnded;

private:
    UPROPERTY()
    TSet<FName> ActiveEscorts;
};
