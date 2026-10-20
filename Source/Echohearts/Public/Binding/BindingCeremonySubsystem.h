// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Binding/BindingTypes.h"
#include "BindingCeremonySubsystem.generated.h"

class AEcoKinCompanion;
class UEncounterSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBindingPhaseChanged, EBindingPhase, OldPhase, EBindingPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBindingCeremonyCompleted, EBindingOutcome, Outcome);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBindingPulseSynced, int32, SyncsSoFar, int32, SyncsRequired);
// Fired when the tamer's link cannot bear the ceremony's cost. The ceremony
// holds at TheOffer until strain decays or the tamer steps back. The link is
// bidirectional: this is the tamer feeling it, not a UI warning.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBindingStrainBlocked);

// The interactive Binding ceremony. Answers UEncounterSubsystem::OnBindingOffered.
// Choreography: Approach -> Attunement (heartbeat sync) -> TheOffer (the EcoKin
// decides) -> Alignment (Anima-Link forms, Kindling seeded).
//
// Canon held verbatim: Binding replaces capture; Resonance Alignment, never
// ownership; the EcoKin CHOOSES; a companion is a relationship, not a
// container; no sacrifice, nothing killed; strain feeds back on the tamer;
// Kindling trust, no XP; the four attributes are never substituted.
//
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-17).
UCLASS()
class ECHOHEARTS_API UBindingCeremonySubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    FBindingCeremonyConfig Config;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Binding")
    FOnBindingPhaseChanged OnPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Binding")
    FOnBindingCeremonyCompleted OnCeremonyCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Binding")
    FOnBindingPulseSynced OnPulseSynced;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Binding")
    FOnBindingStrainBlocked OnStrainBlocked;

    // Player input: attempt to sync with the wild one's pulse. Only meaningful
    // during Attunement. A miss costs nothing; it simply does not build.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Binding")
    void TrySyncPulse();

    // The wild one's heart beats. Driven by animation notifies in the real
    // game; the subsystem's fallback timer calls this until those exist.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Binding")
    void NotifyPulseBeat();

    // The tamer extends Resonance Alignment. The EcoKin decides; the answer
    // is computed from state, attunement, memory, and corruption - never random.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Binding")
    void ExtendOffer();

    // Offer purification to a corrupted wild one. Required before acceptance
    // when corruption is at or above the gate; always optional otherwise.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Binding")
    void OfferPurification();

    // Step back. Always allowed, never punished, no claim kept.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Binding")
    void WithdrawOffer();

    // Danger broke the ceremony (combat started, someone took damage).
    // The wild one flees; no penalty, no bond, no grudge.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Binding")
    void InterruptCeremony();

    UFUNCTION(BlueprintPure, Category = "Echohearts|Binding")
    EBindingPhase GetCurrentPhase() const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Binding")
    bool IsCeremonyActive() const;

    // 0-1 attunement quality: syncs over attempts, softened so early misses
    // do not doom the ceremony.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Binding")
    float GetAttunementQuality() const;

    // Qualitative read of the wild one's willingness for UI ("the wild one
    // seems..."). Exposes the computed value; UI maps it to words, never a bar.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Binding")
    float ReadWillingness() const;

    // Finished ceremonies. Feeds ecology accomplishments and the
    // companion-acquisition matrix ("wild meeting -> Binding" path).
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Binding")
    TArray<FBindingRecord> BindingHistory;

protected:
    // Bound to UEncounterSubsystem::OnBindingOffered. Must be a UFUNCTION for AddDynamic.
    UFUNCTION()
    void HandleBindingOffered(AEcoKinCompanion* WildEcoKin, AActor* Tamer);
    void BeginCeremony(AEcoKinCompanion* WildEcoKin, AActor* Tamer, EEncounterState WildState);
    void SetPhase(EBindingPhase NewPhase);
    void OnApproachFinished();
    void OnPulseTick();
    void OnAttunementTimeout();
    void OnOfferDeliberated();
    float ComputeWillingness() const;
    float GetCorruptionLevel() const;
    int32 CountPeacefulMeetings(int32 EcoKinID) const;
    bool PayStrain(float Amount);
    bool IsStrainBlocked() const;
    void FinishCeremony(EBindingOutcome Outcome);
    void RestoreGamePhase();
    UEncounterSubsystem* GetEncounterSubsystem() const;

    struct FActiveBindingCeremony
    {
        TWeakObjectPtr<AEcoKinCompanion> WildEcoKin;
        TWeakObjectPtr<AActor> Tamer;
        EBindingPhase Phase = EBindingPhase::None;
        EEncounterState WildState = EEncounterState::Calm;
        int32 EcoKinID = 0;
        int32 PulseSyncs = 0;
        int32 PulseAttempts = 0;
        float LastBeatTime = -1.0f;
        float CeremonyStartTime = 0.0f;
        bool bPurifiedDuringCeremony = false;
        bool bStrainHoldNotified = false;
    };

    FActiveBindingCeremony Active;
    FTimerHandle PulseTimerHandle;
    FTimerHandle PhaseTimerHandle;
    TMap<int32, float> LastOfferTimeByEcoKinID;
};
