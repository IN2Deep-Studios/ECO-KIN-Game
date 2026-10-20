// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StormChaseSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStormIntensity, FName, StormId, float, Intensity);

// Storm Chasing: ride storm fronts with the Elderwing. Voltic
// companions drink the lightning, rare storm-EcoKin appear, and the
// Stormrod overcharges — power and wonder, at speed.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UStormChaseSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    bool StartChase(FName StormId);

    // Seconds riding the front; intensity 0-1 drives encounters + charge.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportRiding(FName StormId, float Intensity);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void EndChase(FName StormId, bool bRodeItOut);

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnStormIntensity OnStormIntensity;

private:
    UPROPERTY()
    TSet<FName> ActiveStorms;
};
