// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "EcoKin/EcoKinCoreAttributes.h"
#include "ResonanceShardData.generated.h"

// Resonance Shard data asset.
// Locked canon: rare Resonance Strains receive REDISTRIBUTED (never inflated)
// stat budgets plus unique mutation nodes. BudgetRedistribution components must
// sum to zero; editor validation enforces it.
// Shards never grant ordinary evolution, and Nature-style conditional mutations
// are flagged via bConditionalMutationOnly.
UCLASS(BlueprintType)
class ECHOHEARTS_API UResonanceShardData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|ResonanceShard")
    FName ShardID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|ResonanceShard")
    EEcoKinEssence EssenceAffinity = EEcoKinEssence::Aura;

    // Redistributed stat budget. Positive and negative deltas must sum to 0.0.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|ResonanceShard")
    FEcoKinCoreAttributes BudgetRedistribution;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|ResonanceShard")
    TArray<FName> MutationNodeIDs;

    // True for Nature-style shards: conditional mutations only, never ordinary evolution.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|ResonanceShard")
    bool bConditionalMutationOnly = false;

    // Returns true when the redistribution is budget-neutral (sums to ~0).
    UFUNCTION(BlueprintPure, Category = "Echohearts|ResonanceShard")
    bool IsBudgetNeutral() const;

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
