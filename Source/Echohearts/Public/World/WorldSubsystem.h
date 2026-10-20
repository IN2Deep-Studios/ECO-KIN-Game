// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "World/WorldTypes.h"
#include "WorldSubsystem.generated.h"

// Owns regions, restoration progress, weather, and travel. Travel is flight
// or ride — NEVER teleportation. Fast travel means a wind-current flight or
// a bonded mount's run, with the world streaming past.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API UWorldSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|World")
    void RegisterRegion(const FRegionInfo& Region);

    UFUNCTION(BlueprintPure, Category = "Echohearts|World")
    bool TryGetRegion(FName RegionId, FRegionInfo& OutRegion) const;

    // Records restoration progress in a region (0-100). Broadcasts when a
    // region hits 100 — a healed region changes its music, weather bias, and
    // the Eco-Kin who live there.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|World")
    void AddRestoration(FName RegionId, float Amount);

    // Begins travel to a region by flight or by riding a bonded Eco-Kin.
    // There is no teleport: the journey is real and streamed.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|World")
    bool BeginTravel(FName DestinationRegionId, bool bByFlight);

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|World")
    FName CurrentRegionId = NAME_None;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|World")
    bool bTraveling = false;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRegionRestored, FName, RegionId);

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|World")
    FOnRegionRestored OnRegionRestored;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTravelComplete, FName, RegionId, bool, bByFlight);

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|World")
    FOnTravelComplete OnTravelComplete;

private:
    UPROPERTY()
    TMap<FName, FRegionInfo> Regions;
};
