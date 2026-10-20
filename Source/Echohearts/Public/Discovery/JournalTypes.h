// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "JournalTypes.generated.h"

// Behavior tags the Field Journal recognizes. Observation never requires
// contact — watching from cover is valid research.
UENUM(BlueprintType)
enum class EObservationBehavior : uint8
{
    Feeding    UMETA(DisplayName = "Feeding"),
    Resting    UMETA(DisplayName = "Resting"),
    Call       UMETA(DisplayName = "Call"),
    Play       UMETA(DisplayName = "Play"),
    Territorial UMETA(DisplayName = "Territorial"),
    StateCalm  UMETA(DisplayName = "Calm State"),
    StateRage  UMETA(DisplayName = "Rage State"),
    StateAlpha UMETA(DisplayName = "Alpha State"),
    StateBlight UMETA(DisplayName = "Blight State"),
    StateHollow UMETA(DisplayName = "Hollow State")
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FJournalObservation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    int32 EcoKinID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    EObservationBehavior Behavior = EObservationBehavior::Feeding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FName RegionTag = NAME_None;

    // The player's own words. The Journal is theirs.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText PlayerNote;
};

// A research task: document N observations matching a filter. Breadth over
// grinding — duplicates of the same species+behavior don't count twice.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FResearchTaskDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FName TaskId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText Description;

    // Empty = any species. Otherwise only these Dex IDs count.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    TArray<int32> SpeciesFilter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    TArray<EObservationBehavior> BehaviorFilter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    int32 TargetCount = 3;

    // Kindling trust awarded to the active partner on completion.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    float KindlingReward = 4.0f;
};
