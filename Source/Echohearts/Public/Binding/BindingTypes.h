// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Encounter/EncounterTypes.h"
#include "BindingTypes.generated.h"

// The interactive Binding ceremony: where Resonance Alignment is offered and
// the EcoKin CHOOSES. Binding replaces capture. A companion is a relationship,
// not a container. Nothing here coerces, consumes, or kills.
// Canon: Frequency Tamer / Core-Binder offers; the wild EcoKin decides.

// Choreography of one ceremony, in order. Purification is a tamer action
// available during Attunement, not a separate phase.
UENUM(BlueprintType)
enum class EBindingPhase : uint8
{
    None        UMETA(DisplayName = "None"),
    Approach    UMETA(DisplayName = "Approach"),     // Tamer enters; the wild one is invited, never pulled
    Attunement  UMETA(DisplayName = "Attunement"),   // Heartbeat sync; willingness is earned here
    TheOffer    UMETA(DisplayName = "The Offer"),    // Resonance Alignment extended; the EcoKin decides
    Alignment   UMETA(DisplayName = "Alignment"),    // Anima-Link forms; Kindling seeded
    Complete    UMETA(DisplayName = "Complete")
};

// How a ceremony ended. Declined/Withdrawn/Interrupted carry no penalty and
// no claim: an unanswered offer is never a trap.
UENUM(BlueprintType)
enum class EBindingOutcome : uint8
{
    None             UMETA(DisplayName = "None"),
    Aligned          UMETA(DisplayName = "Aligned"),           // The EcoKin chose yes; Bond formed
    Declined         UMETA(DisplayName = "Declined"),          // The EcoKin chose no; withdrew with dignity
    WithdrawnByTamer UMETA(DisplayName = "Withdrawn By Tamer"), // Tamer stepped back; always allowed
    Interrupted      UMETA(DisplayName = "Interrupted"),      // Danger broke the ceremony; the wild one fled
    RefusedByCanon   UMETA(DisplayName = "Refused By Canon")  // Never-bindable (e.g. Nature); walks its own path
};

// Designer-tunable ceremony choreography. Every number below is a proposal
// Tay can veto; nothing here is canon law.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FBindingCeremonyConfig
{
    GENERATED_BODY()

    // Heartbeat sync: how many matched pulses complete Attunement.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    int32 RequiredPulseSyncs = 3;

    // Timing window around each pulse beat that counts as a sync (seconds).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float SyncWindowSeconds = 0.25f;

    // Fallback pulse interval (seconds). Real beats come from animation
    // notifies via NotifyPulseBeat; this timer keeps the ceremony alive
    // until those notifies exist.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float PulseIntervalSeconds = 1.6f;

    // Attunement gives up gently after this long with no sync (seconds).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float AttunementTimeoutSeconds = 60.0f;

    // Length of the Approach beat before Attunement begins (seconds).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float ApproachDurationSeconds = 4.0f;

    // Deliberation beat after the offer is extended (seconds).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float OfferDeliberationSeconds = 5.0f;

    // Link strain costs. The link is bidirectional: the tamer feels every cost.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float StrainPerSyncAttempt = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float AlignmentStrainCost = 12.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float PurificationStrainCost = 8.0f;

    // Corruption at or above this level blocks acceptance until the tamer
    // offers purification (0-100 scale, same as the Corruption subsystem).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float PurificationGateLevel = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float PurificationAmount = 45.0f;

    // Willingness model. The EcoKin's choice is computed, transparent, and
    // never random: base by state + attunement + remembered kindness - corruption.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float WillingnessThreshold = 0.55f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    TMap<EEncounterState, float> BaseWillingnessByState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float AttunementWeight = 0.30f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float MemoryBonusPerMeeting = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float MemoryBonusCap = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float CorruptionWeight = 0.30f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float PurificationWillingnessBonus = 0.15f;

    // Kindling seed on acceptance: trust starts here, never at zero (the
    // ceremony WAS the first trust), never high (trust is earned, not granted).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float KindlingSeedBase = 12.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float KindlingSeedAttunementBonus = 13.0f;

    // After a decline, the same wild one cannot be re-offered until this
    // much world time passes (seconds). Not a grind timer: a dignity timer.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    float OfferCooldownSeconds = 86400.0f;

    // Dex IDs that can never be bound. Designer data; Nature is the canonical
    // entry per the encounter-state canon lock (never Alpha/Omega/capturable).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Binding")
    TArray<int32> NeverBindableIDs;

    FBindingCeremonyConfig()
    {
        BaseWillingnessByState.Add(EEncounterState::Calm, 0.70f);
        BaseWillingnessByState.Add(EEncounterState::Hollow, 0.50f);
        BaseWillingnessByState.Add(EEncounterState::Alpha, 0.45f);
        BaseWillingnessByState.Add(EEncounterState::Rage, 0.30f);
        BaseWillingnessByState.Add(EEncounterState::Blight, 0.20f);
    }
};

// One finished ceremony, for history, ecology accomplishments, and the
// companion-acquisition matrix ("wild meeting -> Binding" path).
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FBindingRecord
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Binding")
    int32 EcoKinID = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Binding")
    EEncounterState WildState = EEncounterState::Calm;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Binding")
    EBindingOutcome Outcome = EBindingOutcome::None;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Binding")
    float AttunementQuality = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Binding")
    float WillingnessAtOffer = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Binding")
    float TrustSeeded = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Binding")
    bool bPurifiedDuringCeremony = false;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Binding")
    float CeremonyDurationSeconds = 0.0f;
};
