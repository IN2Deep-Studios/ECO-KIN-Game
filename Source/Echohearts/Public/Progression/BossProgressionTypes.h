// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "BossProgressionTypes.generated.h"

// The eras of the world. Bosses don't just fall — the world changes when
// they do. Each era is a promise the restoration is working.
UENUM(BlueprintType)
enum class EWorldEra : uint8
{
    // The beginning: small Sanctuaries, wounded wilds.
    DawnOfRestoration    UMETA(DisplayName = "Dawn of Restoration"),
    // Blightstorm falls: the storm belt is safe, flight routes open.
    SkiesClear           UMETA(DisplayName = "Skies Clear"),
    // Pyrewyrm falls: blight recedes across the fire-scarred regions.
    LandCools            UMETA(DisplayName = "Land Cools"),
    // The Shadow Beast Prime falls: blight recedes worldwide, new regions open.
    WorldBreathesEasier  UMETA(DisplayName = "World Breathes Easier"),
    // The Society Paragon falls: the restoration era begins.
    RestorationEra       UMETA(DisplayName = "Restoration Era")
};

// One phase of a boss fight. Phases are entered when the boss's health drops
// to or below ThresholdPercent — and the fight always tells the tamer it is
// changing. Methods borrowed from the best of the genre: add-gated
// vulnerability, arena-forcing mechanics, strict alternating cycles.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FBossPhaseDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    int32 PhaseIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FString PhaseName;

    // Health % at which this phase begins. 100 = the fight's opening.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    float ThresholdPercent = 100.0f;

    // Behavior tags: "AddSpawn", "Invulnerable", "ArenaHazard", "SafeZone",
    // "AlternatingCycle", "Enrage", "Darkness", "PurifiableAdd", "RiteFinale".
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    TArray<FString> BehaviorTags;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FString Description;

    // Adds spawned in this phase (may be empty). Corrupted adds can be
    // purified mid-fight, not only destroyed.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    TArray<FString> Adds;

    // Special rule for the phase, e.g. "Clear all adds to proceed",
    // "Protect the rite circle for 60 seconds".
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FString PhaseRule;
};

// Where the fight happens. The arena is half the fight: safe zones,
// hazards, and geometry are mechanics, not decoration.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FArenaDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FName ArenaId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FString Description;

    // Arena mechanics: "MovingSafeZone", "FloorHazard", "Darkness",
    // "LaneBased", "DestructibleCover", "GeometryTelegraph".
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    TArray<FString> Mechanics;
};

// One boss in the ordered progression. Every boss here already exists in
// the game: the Seven Beasts, the corrupted alphas of the Twelve Wounded,
// Shadow Beasts, and champions of the secret societies.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FBossDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FName BossId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FString Title;

    // Required bosses gate the story; optional ones reward the curious.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    bool bRequired = true;

    // Position in the sequence. A boss may only be challenged once every
    // REQUIRED boss before it has fallen.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    int32 OrderIndex = 0;

    // For corrupted alphas: the base Dex ID of the Twelve Wounded form.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    int32 CorruptedDexID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FString UnlockHint;

    // World-state change fired when this boss falls (may be empty).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FName WorldTransitionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FString WorldTransitionDescription;

    // The arena this boss is fought in. The arena is half the fight.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FArenaDef Arena;

    // Phases in threshold order (100 first). The fight tells the tamer
    // when it changes.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    TArray<FBossPhaseDef> Phases;

    // How this boss is defeated without killing: "Subdued" (Beasts and
    // corrupted alphas — purifiable), "DrivenOff" (human bosses),
    // "Purified" (Shadow Beast Prime — the rite, not the blow).
    // Nothing sentient is ever killed.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Bosses")
    FString DefeatKind;
};
