// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Discovery/CampfireTypes.h"
#include "CampfireSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMealCooked, FName, MealId, const TMap<FName, int32>&, IngredientsUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMealShared, FName, MealId, float, TrustGain, float, StrainRecovery);

// Campfire Care: cook meals from foraged ingredients, share them with
// partners. Trust gains, strain recovers, Hollow stays away. Cooking is
// care, not consumption — ingredients are gathered, never taken by force.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UCampfireSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    void RegisterMeal(const FMealDef& Def);

    // Registers a resource ID as foraged/gathered (legal ingredient).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    void RegisterForagedIngredient(FName ResourceId);

    // Cooks a meal from the provided ingredients. Returns false if the meal
    // is unknown, ingredients are short, or anything wasn't foraged.
    // The inventory system listens to OnMealCooked to deduct.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    bool CookMeal(FName MealId, const TMap<FName, int32>& ProvidedIngredients);

    // Shares a cooked meal with N partners present. Broadcasts the care.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    bool ShareMeal(FName MealId, int32 PartnersPresent);

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Discovery")
    FOnMealCooked OnMealCooked;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Discovery")
    FOnMealShared OnMealShared;

private:
    UPROPERTY()
    TMap<FName, FMealDef> Meals;

    UPROPERTY()
    TSet<FName> ForagedIngredients;

    UPROPERTY()
    TSet<FName> CookedMeals;
};
