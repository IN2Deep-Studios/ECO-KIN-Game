// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "EncounterTypes.generated.h"

// Encounter STATES (locked canon): Rage / Alpha / Blight / Hollow are states a
// wild EcoKin can be met in — never separate species. Calm is the baseline.
// Omega is a Humanoid-Kin classification and never spawns as a wild encounter.
// Nature is never Alpha or Omega (enforced by designer data, see spawner).
UENUM(BlueprintType)
enum class EEncounterState : uint8
{
    Calm   UMETA(DisplayName = "Calm"),
    Rage   UMETA(DisplayName = "Rage"),
    Alpha  UMETA(DisplayName = "Alpha"),
    Blight UMETA(DisplayName = "Blight"),
    Hollow UMETA(DisplayName = "Hollow")
};

// The beats of a single wild meeting, from first notice to resolution.
UENUM(BlueprintType)
enum class EEncounterPhase : uint8
{
    Unaware        UMETA(DisplayName = "Unaware"),
    Aware          UMETA(DisplayName = "Aware"),
    Engaged        UMETA(DisplayName = "Engaged"),
    BindingOffered UMETA(DisplayName = "Binding Offered"),
    Resolved       UMETA(DisplayName = "Resolved")
};

// How a wild meeting ends. Bound and Befriended are the warm paths.
// Nothing sentient is ever killed: a beaten wild EcoKin withdraws and flees
// to recover — retreat with dignity, always.
UENUM(BlueprintType)
enum class EEncounterResolution : uint8
{
    None      UMETA(DisplayName = "None"),
    Bound     UMETA(DisplayName = "Bound"),      // Resonance Alignment accepted
    Befriended UMETA(DisplayName = "Befriended"), // a Hollow one accepts care, stays wild
    Repelled  UMETA(DisplayName = "Repelled"),   // soothed and driven off, unharmed
    Fled      UMETA(DisplayName = "Fled"),       // it chose to leave
    Withdrawn UMETA(DisplayName = "Withdrawn")   // beaten in a duel — retreats, never dies
};

// One species row in a region's spawn table. Weight is relative; only the
// states listed here may roll for this species in this region.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FEcoSpawnEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    int32 EcoKinID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    float Weight = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    TArray<EEncounterState> AllowedStates;

    // Biological maturity at spawn, 0 (juvenile) to 1 (elder). Scales size and
    // the four attributes. Maturity, never "level."
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MaturityMin = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MaturityMax = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    int32 PackMin = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    int32 PackMax = 1;
};

// A region's full wild population definition. Designers fill one per region;
// spawner actors in the world consume it.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FEcoSpawnTable
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    FName RegionTag = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    TArray<FEcoSpawnEntry> Entries;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    float SpawnIntervalSeconds = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    int32 MaxActivePopulation = 8;

    // Wild EcoKin farther than this from every tamer are released back to the
    // wild (despawned). They were never yours.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    float DespawnDistance = 6000.0f;
};

// The record of one finished meeting. Feeds ecology accomplishments and the
// companion-acquisition matrix ("wild meeting" path).
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FEncounterRecord
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Encounter")
    int32 EcoKinID = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Encounter")
    EEncounterState State = EEncounterState::Calm;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Encounter")
    EEncounterResolution Resolution = EEncounterResolution::None;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Encounter")
    float DurationSeconds = 0.0f;
};
