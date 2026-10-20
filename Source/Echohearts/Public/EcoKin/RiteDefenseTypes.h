// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "RiteDefenseTypes.generated.h"

// One wave approaching the rite circle.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FDefenseWave
{
    GENERATED_BODY()

    // Dex IDs approaching in this wave.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Rite")
    TArray<int32> SpeciesIds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Rite")
    int32 CountPerSpecies = 1;

    // Seconds after defense start when this wave arrives.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Rite")
    float ArriveAtSeconds = 10.0f;
};

UENUM(BlueprintType)
enum class ERiteDefenseState : uint8
{
    Inactive  UMETA(DisplayName = "Inactive"),
    Preparing UMETA(DisplayName = "Preparing"),
    Active    UMETA(DisplayName = "Active"),
    Complete  UMETA(DisplayName = "Complete"),
    Broken    UMETA(DisplayName = "Broken")
};

// A rite defense: hold the circle while a Growth Rite or purification
// completes. Waves are data; nothing dies — beaten wild ones withdraw,
// soothed ones are befriended. If the circle breaks, the rite pauses:
// retry, no penalty beyond time.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FRiteDefenseDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Rite")
    FName DefenseId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Rite")
    FText RiteName;

    // Seconds the circle must hold.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Rite")
    float HoldDurationSeconds = 60.0f;

    // Radius of the rite circle in cm.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Rite")
    float CircleRadiusCm = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Rite")
    TArray<FDefenseWave> Waves;
};
