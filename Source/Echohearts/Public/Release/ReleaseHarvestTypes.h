// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "ReleaseHarvestTypes.generated.h"

// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-14).
// Tay's canon (2026-10-14): willing RELEASE of a duplicate EcoKin yields SHED/GIFTED
// bodily resources — molted scales, shed feathers, fallen petals, sap, spores,
// crystal shards, essence traces. Nothing is killed or consumed. This is the
// part-based crafting loop METHOD (ecology-first gathering), never another game's
// content: no external names, monsters, items, or fiction appear anywhere here.

// Resource star tiers, ascending. A resource starts at Yellow (no star earned yet);
// every 100 lifetime points grants the next star, capped at RedStar.
UENUM(BlueprintType)
enum class EResourceStarTier : uint8
{
    Yellow   UMETA(DisplayName = "Yellow"),
    Bronze   UMETA(DisplayName = "Bronze"),
    Silver   UMETA(DisplayName = "Silver"),
    Gold     UMETA(DisplayName = "Gold"),
    Platinum UMETA(DisplayName = "Platinum"),
    Purple   UMETA(DisplayName = "Purple"),
    RedStar  UMETA(DisplayName = "Red Star")
};

// Tay's spec: 100 units of progress grant a star. Different resources accumulate
// points at different rates via FHarvestNode::PointsPerUnit.
static constexpr int32 UnitsPerStar = 100;

inline EResourceStarTier GetStarTierForLifetimePoints(int32 LifetimePoints)
{
    const int32 TierIndex = FMath::Clamp(
        LifetimePoints / UnitsPerStar,
        0,
        static_cast<int32>(EResourceStarTier::RedStar));
    return static_cast<EResourceStarTier>(TierIndex);
}

inline FText GetStarTierDisplayName(EResourceStarTier Tier)
{
    switch (Tier)
    {
    case EResourceStarTier::Yellow:   return FText::FromString(TEXT("Yellow"));
    case EResourceStarTier::Bronze:   return FText::FromString(TEXT("Bronze"));
    case EResourceStarTier::Silver:   return FText::FromString(TEXT("Silver"));
    case EResourceStarTier::Gold:     return FText::FromString(TEXT("Gold"));
    case EResourceStarTier::Platinum: return FText::FromString(TEXT("Platinum"));
    case EResourceStarTier::Purple:   return FText::FromString(TEXT("Purple"));
    case EResourceStarTier::RedStar:  return FText::FromString(TEXT("Red Star"));
    default:                          return FText::FromString(TEXT("Yellow"));
    }
}

// One stack of a shed/gifted/harvested resource. LifetimePoints is the tamer's
// all-time accumulation for this resource ID and drives the star tier.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FResourceStack
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    FName ResourceId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    EEcoKinOrder SourceOrder = EEcoKinOrder::Zoa;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    EEcoKinEssence SourceEssence = EEcoKinEssence::Flora;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    int32 Quantity = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    int32 LifetimePoints = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    EResourceStarTier StarTier = EResourceStarTier::Yellow;
};

// Everything a willing release (or a harvest node) produces in one event.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FReleaseYield
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    TArray<FResourceStack> Stacks;

    void AddStack(const FResourceStack& Stack)
    {
        for (FResourceStack& Existing : Stacks)
        {
            if (Existing.ResourceId == Stack.ResourceId)
            {
                Existing.Quantity += Stack.Quantity;
                Existing.LifetimePoints = FMath::Max(Existing.LifetimePoints, Stack.LifetimePoints);
                Existing.StarTier = GetStarTierForLifetimePoints(Existing.LifetimePoints);
                return;
            }
        }
        Stacks.Add(Stack);
    }
};

// A harvestable world flora node. Flora EcoKin shed petals/sap/spores on release;
// the living world also offers plant resources directly, at per-node rates.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FHarvestNode
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    FName NodeId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    FName ResourceId = NAME_None;

    // Units granted per successful harvest.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    int32 YieldPerHarvest = 1;

    // Seconds before the node can be harvested again. The world regrows; nodes
    // are never permanently depleted (restoration thesis).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    float RespawnSeconds = 300.0f;

    // Per-resource leveling rate: different resources level at different rates.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Release")
    float PointsPerUnit = 1.0f;
};
