// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Companion/WorkAptitudeTypes.h"
#include "WorkforceTypes.generated.h"

UENUM(BlueprintType)
enum class EJobCategory : uint8
{
    Harvest UMETA(DisplayName = "Harvest"),
    Craft   UMETA(DisplayName = "Craft"),
    Defense UMETA(DisplayName = "Defense"),
    Care    UMETA(DisplayName = "Care"),
    Watch   UMETA(DisplayName = "Watch"),
    Lore    UMETA(DisplayName = "Lore")
};

// A base job: one resident's work that needs one companion's hands.
// The lean goes both ways — the resident's craft needs the companion,
// and the companion's health needs the resident's care. An empty job
// is a promise the base can't keep.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FBaseJob
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Workforce")
    FName JobId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Workforce")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Workforce")
    EJobCategory Category = EJobCategory::Harvest;

    // The resident this job supports (mender, artificer, chronicler,
    // bondkeeper, purifier, scout). If they're in the infirmary, the
    // job goes quiet — nobody to lean on.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Workforce")
    FName ResidentId = NAME_None;

    // Essence affinity, e.g. "Pyre" for the forgehand. Matching
    // companions work 50% better — the right hands for the work.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Workforce")
    FName PreferredEssence = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Workforce")
    FText Description;

    // Wealth per hour when the job is active and filled.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Workforce")
    float WealthPerHour = 5.0f;

    // Defense rating contributed when active (Defense jobs).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Workforce")
    float DefenseBonus = 0.0f;

    // The aptitude this job wants (Emberheart for the forgehand...).
    // Output scales with the assigned companion's tier in it.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Workforce")
    EWorkAptitude RequiredAptitude = EWorkAptitude::None;

    // Assigned companion (0 = vacant).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Workforce")
    int32 AssignedEcoKinID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Workforce")
    bool bAffinityMatch = false;

    // The assigned companion's tier in RequiredAptitude (0 = lacks it).
    // Set at assignment; drives the efficiency multiplier.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echohearts|Workforce")
    int32 AptitudeTier = 0;
};
