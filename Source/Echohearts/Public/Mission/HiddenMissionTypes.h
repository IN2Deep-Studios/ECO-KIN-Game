// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "HiddenMissionTypes.generated.h"

// One stage of a hidden mission: discovery, restoration deeds, puzzles, and
// finally the Binding offer. Stages advance in order; a stage is done when
// its objective count is met.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FHiddenMissionStage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    int32 TargetCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    int32 CurrentCount = 0;

    bool IsDone() const { return CurrentCount >= TargetCount; }
};

// A hidden mission definition. The exclusive EcoKin is obtained ONLY through
// this mission's final Binding offer — never from the wild — until the
// mission completes, at which point the species joins the wild population of
// its habitat regions (see UHiddenMissionSubsystem::CompleteMission).
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FHiddenMissionDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    FName MissionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    FText Title;

    // Dex ID of the exclusive EcoKin (e.g. 513 Prismara, 514 Chromapaw).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    int32 ExclusiveEcoKinID = 0;

    // Region tags (match AWildEcoKinSpawner region tags) where the species
    // appears in the wild after the mission completes. The world map listens
    // for the unlock and drops presence markers in these regions.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    TArray<FName> HabitatRegionTags;

    // Name of the hidden sanctuary unlocked at the final stage
    // (e.g. "Prism Paws Sanctuary", "Dawnpaw Sanctum").
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    FText SanctuaryName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    TArray<FHiddenMissionStage> Stages;

    // Weight at which the species enters wild spawner tables post-unlock.
    // Low: exclusives stay rare.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission", meta = (ClampMin = "0.0"))
    float WildUnlockWeight = 0.35f;
};
