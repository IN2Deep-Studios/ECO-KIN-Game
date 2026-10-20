// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sanctuary/VesselTypes.h"
#include "VesselSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVesselUpgraded, int32, NewTier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTerraceCleansed, int32, CleansedCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRosterChanged);

// The Elderwing's systems: the roost (reserve companions), the active
// battle roster, the sky-gardens' cleansing, and the vessel's own
// Deepen/Widen upgrades. No NPCs aboard — companions roost here, the
// Tamer captains, and a Galecalling companion runs messages.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UVesselSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Vessel")
    void NameVessel(const FText& NewName);

    // She flies; she never teleports. Boarding is flight or ride.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Vessel")
    void LaunchVessel();

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Vessel")
    void DockVessel(EVesselState Dock);

    // Roster: companions roost aboard (reserve) or stand in the active
    // battle team. The active team is chosen here, nowhere else.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Vessel")
    bool AssignToRoost(int32 EcoKinID);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Vessel")
    bool AssignToActiveTeam(int32 EcoKinID);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Vessel")
    bool RemoveFromVessel(int32 EcoKinID);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Vessel")
    TArray<int32> GetRoost() const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Vessel")
    TArray<int32> GetActiveTeam() const;

    // Cleanses one ash-covered sky-garden terrace. Gated on real
    // restoration: the game passes Rebearth's healed percentage, and a
    // terrace opens per threshold crossed. Restoration made visible.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Vessel")
    bool CleanseTerrace(float RegionRestorationPercent);

    // Deepen (richer roost: faster recovery, deeper gardens) or Widen
    // (more berths, more terraces). Shed/gifted costs, never coins —
    // there is no new currency.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Vessel")
    bool UpgradeVessel(EUpgradePath Path);

    // Roost recovery bonus multiplier for the care subsystem to apply.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Vessel")
    float GetRoostRecoveryBonus() const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Vessel")
    FVesselData GetVesselData() const { return Vessel; }

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Vessel")
    FOnVesselUpgraded OnVesselUpgraded;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Vessel")
    FOnTerraceCleansed OnTerraceCleansed;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Vessel")
    FOnRosterChanged OnRosterChanged;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Vessel")
    int32 ActiveTeamMaxSize = 6;

    // Restoration % per terrace cleansed.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Vessel")
    float RestorationPerTerrace = 10.0f;

private:
    UPROPERTY()
    FVesselData Vessel;

    UPROPERTY()
    TArray<int32> Roost;

    UPROPERTY()
    TArray<int32> ActiveTeam;
};
