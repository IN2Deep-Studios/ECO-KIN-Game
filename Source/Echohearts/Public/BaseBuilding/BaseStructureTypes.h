// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "BaseStructureTypes.generated.h"

UENUM(BlueprintType)
enum class EStructureCategory : uint8
{
    Anchor      UMETA(DisplayName = "Anchor"),
    Rest        UMETA(DisplayName = "Rest"),
    Nourishment UMETA(DisplayName = "Nourishment"),
    Purification UMETA(DisplayName = "Purification"),
    Harvest     UMETA(DisplayName = "Harvest"),
    Morale      UMETA(DisplayName = "Morale"),
    Defense     UMETA(DisplayName = "Defense"),
    Power       UMETA(DisplayName = "Power"),
    Play        UMETA(DisplayName = "Play")
};

UENUM(BlueprintType)
enum class EUpgradePath : uint8
{
    Deepen UMETA(DisplayName = "Deepen"), // Stronger effect.
    Widen  UMETA(DisplayName = "Widen")   // Larger radius / more served.
};

// One resource cost line. All costs are shed/gifted resources — the base is
// built from what was given, never what was taken.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FStructureCost
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    FName ResourceId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    int32 Units = 0;
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FBaseStructureDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    FName StructureId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    EStructureCategory Category = EStructureCategory::Anchor;

    // Max tier (1-3). Tier 1 placed; Deepen/Widen advance it.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    int32 MaxTier = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    TArray<FStructureCost> BuildCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    TArray<FStructureCost> UpgradeCost;

    // Contribution to the base defense rating (raids).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    float DefenseRating = 0.0f;

    // Strain recovery per hour for resting companions (Rest/Morale).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    float StrainRecoveryPerHour = 0.0f;

    // Hit points. At 0 the structure is destroyed and must be rebuilt.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    float MaxHitPoints = 100.0f;
};

// A placed instance.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FPlacedStructure
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    FName StructureId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    int32 Tier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    FVector Location = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    float CurrentHitPoints = 100.0f;

    // True when hit points hit zero. A destroyed structure stops working
    // and must be rebuilt from scratch — repairs can't raise the dead.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    bool bDestroyed = false;
};
