// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "BattleTypes.generated.h"

// Battle modes. Combat is REAL-TIME action — never turn-based. Every mode
// runs on the same truth: one deployed EcoKin per Tamer, every strike paid
// through the Anima-Link, nothing sentient ever killed.
UENUM(BlueprintType)
enum class EBattleMode : uint8
{
    // 1v1: Tamer vs Tamer, or Tamer vs NPC duelist. One deployed EcoKin
    // per side. Won by subduing, never by killing.
    Duel     UMETA(DisplayName = "Duel"),
    // Cooperative takedown of a giant corrupted EcoKin (Cataclysm-class).
    // Multiple Tamers coordinate; the giant warps the arena as it fights.
    Raid     UMETA(DisplayName = "Raid"),
    // Consensual practice. Strikes are pulled — no real harm — and the
    // bout builds Kindling (trust) instead of wounds. Training is bonding.
    Sparring UMETA(DisplayName = "Sparring")
};

UENUM(BlueprintType)
enum class EBattleResult : uint8
{
    None        UMETA(DisplayName = "None"),
    Victory     UMETA(DisplayName = "Victory"),     // opponent subdued / yielded
    Defeat      UMETA(DisplayName = "Defeat"),       // own partner withdrawn
    Draw        UMETA(DisplayName = "Draw"),         // mutual withdrawal / timer
    Purified    UMETA(DisplayName = "Purified"),     // raid target brought back
    Interrupted UMETA(DisplayName = "Interrupted")   // fled, disconnected, conceded
};

// Harmonic Momentum: the reward for fighting IN the bond instead of
// through it. The Anima-Link pulses on a steady rhythm; activating an
// ability inside the pulse window builds momentum (0-100). Missing the
// window lets it decay. At thresholds, Resonance Techniques unlock:
// 40 = Attuned Technique, 70 = Harmonic Technique, 100 = Sovereign Technique.
// This is the real-time adaptation of rhythm-synced combat — timing, not turns.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FHarmonicMomentum
{
    GENERATED_BODY()

    // 0-100.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Battle")
    float Momentum = 0.0f;

    // Consecutive on-pulse activations.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Battle")
    int32 PulseStreak = 0;

    // Seconds between link pulses. The heartbeat of the bond.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Battle")
    float PulseInterval = 2.0f;

    // How close to the pulse an activation must land to count (seconds).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Battle")
    float PulseWindow = 0.25f;

    int32 UnlockedTechniqueTier() const
    {
        if (Momentum >= 100.0f) return 3;
        if (Momentum >= 70.0f) return 2;
        if (Momentum >= 40.0f) return 1;
        return 0;
    }
};

// One essence-chain definition: two essences struck in rapid sequence
// trigger a combined effect greater than either alone. Chains reward
// coordination — in raids, two Tamers can each strike half the chain.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FEssenceChainDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Battle")
    FName ChainId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Battle")
    FString ChainName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Battle")
    EEcoKinEssence FirstEssence = EEcoKinEssence::Flora;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Battle")
    EEcoKinEssence SecondEssence = EEcoKinEssence::Flora;

    // Seconds within which the second strike must land.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Battle")
    float ChainWindow = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Battle")
    FString EffectDescription;
};

// A live chain attempt being tracked.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FEssenceChain
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Battle")
    EEcoKinEssence LastEssence = EEcoKinEssence::Flora;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Battle")
    float LastStrikeTime = -1000.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Battle")
    bool bHasFirst = false;
};
