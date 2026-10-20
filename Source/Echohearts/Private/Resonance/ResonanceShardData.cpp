// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Resonance/ResonanceShardData.h"

bool UResonanceShardData::IsBudgetNeutral() const
{
    const float Sum = BudgetRedistribution.Vibrance
        + BudgetRedistribution.Density
        + BudgetRedistribution.Harmony
        + BudgetRedistribution.Purity;
    return FMath::IsNearlyZero(Sum, 1e-3f);
}

#if WITH_EDITOR
#include "Misc/DataValidation.h"

EDataValidationResult UResonanceShardData::IsDataValid(FDataValidationContext& Context) const
{
    EDataValidationResult Result = Super::IsDataValid(Context);

    if (ShardID.IsNone())
    {
        Context.AddError(FText::FromString(TEXT("Resonance Shard requires a ShardID.")));
        Result = EDataValidationResult::Invalid;
    }

    if (!IsBudgetNeutral())
    {
        // Locked canon: Resonance Strain budgets are redistributed, never inflated.
        Context.AddError(FText::FromString(
            TEXT("BudgetRedistribution must sum to 0 (redistributed, never inflated).")));
        Result = EDataValidationResult::Invalid;
    }

    return Result;
}
#endif
