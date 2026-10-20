// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BlightHuntSubsystem.generated.h"

UENUM(BlueprintType)
enum class EBlightHuntState : uint8
{
    Idle      UMETA(DisplayName = "Idle"),
    Tracking  UMETA(DisplayName = "Tracking"),
    AtSource  UMETA(DisplayName = "AtSource"),
    Cleansing UMETA(DisplayName = "Cleansing"),
    Healed    UMETA(DisplayName = "Healed")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHuntStage, FName, HuntId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHuntEnded, FName, HuntId, bool, bHealed);

// Blight Hunts: track corruption to its source — follow the gray,
// fight the Tainted, then hold the circle while the land is walked
// back. Ends in a Rite Defense: the purification rite, protected.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UBlightHuntSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    bool StartHunt(FName HuntId, FName RegionId);

    // Trail signs found (ash drifts, withered groves, wrong silence).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportTrailFound(FName HuntId, int32 TrailsTotal);

    // At the source: the fight begins; victory starts the cleansing.
    // The game then runs the Rite Defense for the purification rite.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportSourceReached(FName HuntId);

    // Called by the Rite Defense outcome: rite held -> healed.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportRiteHeld(FName HuntId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Adventure")
    EBlightHuntState GetHuntState(FName HuntId) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnHuntStage OnHuntStage;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnHuntEnded OnHuntEnded;

private:
    UPROPERTY()
    TMap<FName, EBlightHuntState> States;

    UPROPERTY()
    TMap<FName, int32> TrailsFound;
};
