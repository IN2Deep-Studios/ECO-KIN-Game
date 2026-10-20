// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Corruption/CorruptionTypes.h"
#include "CorruptionSubsystem.generated.h"

// CORRUPTION — the dark path, and the road back.
//
// A Corruption form is an alternate Ecoform worn when corruption takes hold:
// the same Echoprint, the same creature, expressing its pain. Stats are
// REDISTRIBUTED, never inflated — corruption borrows from Harmony and Purity
// to feed Vibrance and Density. Essence expression slides toward Shade.
//
// The loop: wounded places corrupt the wild ones living in them. Those
// corrupted ones are encountered around the map — fought, avoided, or
// purified. Purification (care, restoration, A.E.G.I.S. rites) walks the
// level back down and restores exactly what was there.
//
// At 100 the Echoheart fractures: the Shadow condition takes hold. A
// fractured one is never killed — it is lost, and must be brought back.
// INTEGRATION (future): SummoningSubsystem should refuse Fractured partners.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-16).
UCLASS()
class ECHOHEARTS_API UCorruptionSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;

    // Registers a designed Corruption form (data-side).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Corruption")
    void RegisterCorruptionForm(const FCorruptionFormDef& FormDef);

    // Marks an EcoKin as standing in a corrupted place. Call with
    // bExposed=false when it leaves.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Corruption")
    void SetZoneExposure(int32 EcoKinID, bool bExposed, ECorruptionSource Source);

    // Direct accumulation (story events, care failures). Amount is 0–100.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Corruption")
    void AccumulateCorruption(int32 EcoKinID, float Amount, ECorruptionSource Source);

    // Purification walks the level back down. Method is a label for the
    // log/UI: "Care", "Restoration", "AEGIS Rite", "Time Apart".
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Corruption")
    void Purify(int32 EcoKinID, float Amount, const FString& Method);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Corruption")
    float GetCorruptionLevel(int32 EcoKinID) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Corruption")
    ECorruptionStage GetCorruptionStage(int32 EcoKinID) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Corruption")
    bool IsCorruptionFormActive(int32 EcoKinID) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Corruption")
    bool TryGetCorruptionForm(int32 BaseDexID, FCorruptionFormDef& OutDef) const;

protected:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
    void ApplyCorruptionForm(int32 EcoKinID, class AEcoKinCompanion* Companion);
    void RevertCorruptionForm(int32 EcoKinID, class AEcoKinCompanion* Companion);
    class AEcoKinCompanion* FindCompanion(int32 EcoKinID) const;

    UPROPERTY()
    TMap<int32, FCorruptionState> CorruptionMap;

    UPROPERTY()
    TMap<int32, FCorruptionFormDef> FormDefs;

    // EcoKin currently standing in corrupted ground.
    UPROPERTY()
    TMap<int32, ECorruptionSource> ExposedSet;

    // Corruption gained per second of exposure.
    float ExposureRate = 1.5f;
    // Corruption form manifests at this level.
    float FormThreshold = 50.0f;
    // Below this, the form falls away on its own.
    float RevertThreshold = 30.0f;
};
