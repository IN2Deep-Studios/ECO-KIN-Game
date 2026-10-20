// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sanctuary/SanctuaryResidentTypes.h"
#include "SanctuaryResidentSubsystem.generated.h"

// SANCTUARY RESIDENTS — Rebearth's people, coming home.
//
// Residents are humans who move into the player's Sanctuary as restoration
// milestones are reached: a Mender after the first Growth Rite, an Artificer
// when shed resources first change hands, a Purifier when corruption is
// first walked back. Each offers services no machine can: healing, crafting
// from gifted resources, purification aid, kindling guidance, lore through
// D.A.H.L.I.A., and knowledge of the wilds.
//
// When the Sanctuary is threatened, residents defend it — themselves and
// the tamer beside them. Their defense grows as restoration % rises: a
// Sanctuary that has healed more of the world is harder to break.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-16).
UCLASS()
class ECHOHEARTS_API USanctuaryResidentSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Registers a resident design (data-side). Ships with the six founding
    // roles; more may arrive as the story grows.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Sanctuary")
    void RegisterResidentDef(const FSanctuaryResidentDef& Def);

    // Reports a world milestone. Any resident whose arrival condition is met
    // moves in; returns the IDs of new arrivals.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Sanctuary")
    TArray<FName> NotifyMilestone(EResidentArrivalKind Kind);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    bool IsMovedIn(FName ResidentId) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    TArray<FName> GetMovedInResidents() const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    bool TryGetResidentDef(FName ResidentId, FSanctuaryResidentDef& OutDef) const;

    // Requests a service from a moved-in resident. ServiceIndex addresses the
    // resident's Services list. Returns a human-readable outcome for UI.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Sanctuary")
    FString RequestService(FName ResidentId, int32 ServiceIndex);

    // Combined Sanctuary defense strength. Base ratings plus a restoration
    // bonus: every resident fights harder for a world that is healing.
    // RestorationPercent is 0–100 across all tracked regions.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    float GetSanctuaryDefense(float RestorationPercent) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    int32 GetResidentCount() const;

protected:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
    void RegisterFoundingResidents();

    UPROPERTY()
    TMap<FName, FSanctuaryResidentDef> ResidentDefs;

    UPROPERTY()
    TMap<FName, FSanctuaryResidentState> ResidentStates;
};
