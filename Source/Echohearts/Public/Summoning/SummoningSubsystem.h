// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Summoning/SummoningTypes.h"
#include "SummoningSubsystem.generated.h"

// SUMMONING through the Eco-Device.
//
// Canon reconciliation: the Eco-Device is the tamer's physical field
// instrument — the hardware running the A.E.G.I.S. intelligence. Summoning is
// the act of CALLING an already-bonded Eco-Kin through the Anima-Link. It is
// not capture, not containment: only a partner you have Bound can be called,
// and a companion is a relationship, not a container.
//
// Every summon pays Link Strain first and Eco-Device charge second. The bond
// can refuse: low trust goes unanswered, a strained link cannot bear the
// call, and ceremonies are never interrupted.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-16).
UCLASS()
class ECHOHEARTS_API USummoningSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Records a completed Binding: this Eco-Kin can now be summoned.
    // (The Binding ceremony itself lives in the ceremony flow; this only
    // tracks the roster.)
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Summoning")
    bool RegisterBond(int32 EcoKinID, AActor* CompanionActor);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Summoning")
    bool RemoveBond(int32 EcoKinID);

    // Calls a bonded partner to the tamer's side via the Eco-Device.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Summoning")
    FSummonResult RequestSummon(int32 EcoKinID);

    // Returns the deployed partner to rest. Free; the bond breathes easier.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Summoning")
    FSummonResult RequestReturn();

    UFUNCTION(BlueprintPure, Category = "Echohearts|Summoning")
    int32 GetDeployedEcoKinID() const { return DeployedEcoKinID; }

    UFUNCTION(BlueprintPure, Category = "Echohearts|Summoning")
    bool IsBonded(int32 EcoKinID) const { return Roster.Contains(EcoKinID); }

    UFUNCTION(BlueprintPure, Category = "Echohearts|Summoning")
    TArray<int32> GetBondedIDs() const;

private:
    float ComputeStrainCost(AActor* Tamer, AActor* Companion) const;
    float GetTrustPercent(int32 EcoKinID) const;
    class AFrequencyTamerCharacter* GetTamer() const;
    bool IsCeremonyLocked() const;

    UPROPERTY()
    TMap<int32, FBondedPartner> Roster;

    int32 DeployedEcoKinID = 0;

    // Tuning (data-side can override later).
    float BaseStrainCost = 10.0f;
    float MaxStrainCost = 30.0f;
    float DeviceChargeCost = 15.0f;
    float MinTrustToAnswer = 15.0f;
};
