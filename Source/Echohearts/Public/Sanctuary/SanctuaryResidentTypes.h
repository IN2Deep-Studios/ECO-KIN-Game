// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SanctuaryResidentTypes.generated.h"

// The roles Rebearth's people take when they come home to the Sanctuary.
// Residents are humans — tamers, healers, makers — never EcoKin.
UENUM(BlueprintType)
enum class ESanctuaryRole : uint8
{
    // Tends the wounded: restores companion health and eases link strain.
    Mender         UMETA(DisplayName = "Mender"),
    // Shapes shed and gifted resources into gear, catalysts, and instruments.
    Artificer      UMETA(DisplayName = "Artificer"),
    // Reads the Forms Registry through D.A.H.L.I.A.; keeps the living record.
    Chronicler     UMETA(DisplayName = "Chronicler"),
    // Teaches the care that builds Kindling: feeding, rest, attention.
    Bondkeeper     UMETA(DisplayName = "Bondkeeper"),
    // Aids purification: walks corruption down alongside the tamer.
    Purifier       UMETA(DisplayName = "Purifier"),
    // Knows the wilds: region intel, territory warnings, pathfinding.
    Scout          UMETA(DisplayName = "Scout")
};

// What must be true in the world before a resident moves in. Every arrival
// is earned by restoration — nobody comes to a Sanctuary that isn't healing.
UENUM(BlueprintType)
enum class EResidentArrivalKind : uint8
{
    // The tamer completes their first Growth Rite.
    FirstGrowthRite      UMETA(DisplayName = "First Growth Rite"),
    // Any region's restoration reaches 50%.
    RegionRestoredHalf   UMETA(DisplayName = "Region Restored Half"),
    // The tamer reverts their first Corruption form through purification.
    FirstPurification    UMETA(DisplayName = "First Purification"),
    // Any bonded partner reaches 80% trust.
    KindlingEighty        UMETA(DisplayName = "Kindling Eighty"),
    // 25 Dex identities registered in the Forms Registry.
    DexMilestone         UMETA(DisplayName = "Dex Milestone"),
    // The tamer gathers their first shed/gifted resources (willing Release).
    FirstShedResources   UMETA(DisplayName = "First Shed Resources")
};

// One resident's design record (data-side).
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FSanctuaryResidentDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    FName ResidentId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    ESanctuaryRole Role = ESanctuaryRole::Mender;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    EResidentArrivalKind Arrival = EResidentArrivalKind::FirstGrowthRite;

    // Services offered once moved in, e.g. "Heal Companion", "Craft Catalyst".
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    TArray<FString> Services;

    // Base contribution to Sanctuary defense. Grows with restoration %.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    float DefenseRating = 10.0f;

    // What must be built before this resident moves in.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    FResidentAnchorDef Anchor;

    // Service-expansion tiers unlocked by progression (Tier 0 = base Services).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    TArray<FResidentServiceTier> ExpansionTiers;

    // How this resident fights when the Sanctuary is threatened.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    FString DefenseBehavior;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    FString WelcomeLine;
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FSanctuaryResidentState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    FName ResidentId = NAME_None;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    bool bMovedIn = false;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    int32 ServicesUsed = 0;
};

// What must be BUILT before a resident will move in. Residents anchor to
// Sanctuary infrastructure: nobody comes to a Sanctuary that isn't ready
// to hold them. Build order: Pylon -> walls -> station -> node.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FResidentAnchorDef
{
    GENERATED_BODY()

    // The Sanctuary's beating heart. Every resident anchors within its range.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    bool bRequiresAuraPylon = true;

    // Their work surface, flavored per role: "Restorative Bench", "Forge",
    // "Archive Desk", "Hearth", "Cleansing Font", "Map Table".
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    FString AnchorStationName;

    // Their rest nook. A resident without rest burns out; services slow.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    bool bRequiresRechargingNode = true;

    // The perimeter must hold a minimum defense before anyone moves in.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    bool bRequiresEnergyWalls = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    FString AnchorNotes;
};

// One service-expansion tier: new services unlocked as the tamer progresses.
// Services grow with the story — a resident met at the start of the journey
// is not the same resident at its end.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FResidentServiceTier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    int32 TierIndex = 0;

    // Progression gate, e.g. "3 bosses defeated", "The World Breathes Easier".
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    FString UnlockCondition;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Sanctuary")
    TArray<FString> Services;
};
