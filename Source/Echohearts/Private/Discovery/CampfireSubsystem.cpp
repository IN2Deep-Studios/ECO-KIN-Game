// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Discovery/CampfireSubsystem.h"

void UCampfireSubsystem::RegisterMeal(const FMealDef& Def)
{
    if (Def.MealId.IsNone() || Def.RequiredIngredients.IsEmpty())
    {
        return;
    }
    Meals.Add(Def.MealId, Def);
}

void UCampfireSubsystem::RegisterForagedIngredient(FName ResourceId)
{
    if (!ResourceId.IsNone())
    {
        ForagedIngredients.Add(ResourceId);
    }
}

bool UCampfireSubsystem::CookMeal(FName MealId, const TMap<FName, int32>& ProvidedIngredients)
{
    const FMealDef* Def = Meals.Find(MealId);
    if (!Def)
    {
        return false;
    }
    for (const auto& Pair : Def->RequiredIngredients)
    {
        // The kitchen rule: foraged only. Nothing taken by force is food.
        if (!ForagedIngredients.Contains(Pair.Key))
        {
            return false;
        }
        const int32* Provided = ProvidedIngredients.Find(Pair.Key);
        if (!Provided || *Provided < Pair.Value)
        {
            return false;
        }
    }

    CookedMeals.Add(MealId);
    OnMealCooked.Broadcast(MealId, Def->RequiredIngredients);
    return true;
}

bool UCampfireSubsystem::ShareMeal(FName MealId, int32 PartnersPresent)
{
    const FMealDef* Def = Meals.Find(MealId);
    if (!Def || !CookedMeals.Contains(MealId) || PartnersPresent <= 0)
    {
        return false;
    }
    CookedMeals.Remove(MealId);
    OnMealShared.Broadcast(MealId, Def->TrustGain, Def->StrainRecovery);
    return true;
}
