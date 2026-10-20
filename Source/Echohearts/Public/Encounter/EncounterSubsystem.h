// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Encounter/EncounterTypes.h"
#include "EncounterSubsystem.generated.h"

class AEcoKinCompanion;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEncounterStarted, AEcoKinCompanion*, WildEcoKin, AActor*, Tamer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEncounterEnded, int32, EcoKinID, EEncounterState, State, EEncounterResolution, Resolution);
// Fired when a tamer offers Binding to a wild EcoKin. The interactive Binding
// ceremony (not yet implemented) will bind here. Until then, an unanswered
// offer is never a trap: the wild one simply waits, and may wander off.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBindingOffered, AEcoKinCompanion*, WildEcoKin, AActor*, Tamer);

// Tracks every live wild meeting in the world and keeps its history.
// This is the on-ramp the Binding ceremony was missing: encounters begin
// here, and Binding offers are announced here for the ceremony to answer.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-17).
UCLASS()
class ECHOHEARTS_API UEncounterSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Encounter")
    FOnEncounterStarted OnEncounterStarted;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Encounter")
    FOnEncounterEnded OnEncounterEnded;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Encounter")
    FOnBindingOffered OnBindingOffered;

    // Called by wild behavior when a meeting truly begins. Idempotent.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Encounter")
    void BeginEncounter(AEcoKinCompanion* WildEcoKin, AActor* Tamer);

    // Called when the meeting ends for any reason. Records history.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Encounter")
    void EndEncounter(AEcoKinCompanion* WildEcoKin, EEncounterResolution Resolution);

    // The tamer offers Binding (Resonance Alignment) to this wild EcoKin.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Encounter")
    void OfferBinding(AEcoKinCompanion* WildEcoKin, AActor* Tamer);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Encounter")
    bool IsInEncounter(AEcoKinCompanion* WildEcoKin) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Encounter")
    int32 GetActiveEncounterCount() const;

    // Finished meetings — feeds ecology accomplishments and the
    // companion-acquisition matrix ("wild meeting" path).
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Encounter")
    TArray<FEncounterRecord> EncounterHistory;

protected:
    virtual void Deinitialize() override;

private:
    struct FActiveEncounter
    {
        TWeakObjectPtr<AActor> Tamer;
        EEncounterPhase Phase = EEncounterPhase::Unaware;
        float StartTime = 0.0f;
        EEncounterState State = EEncounterState::Calm;
        int32 EcoKinID = 0;
    };

    TMap<TWeakObjectPtr<AEcoKinCompanion>, FActiveEncounter> ActiveEncounters;
};
