// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Crafting/CraftingTypes.h"
#include "CraftingSubsystem.generated.h"

// Part-based crafting runtime. Owns the tamer's resource inventory (fed by
// willing releases and flora harvests), gates recipes on Tay's star tiers, and
// consumes ingredients on craft. Canon lock: no recipe may produce a weapon
// that harms EcoKin; products are A.E.G.I.S. modules, protective tamer gear,
// Sanctuary structures, and consumables.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-14).
UCLASS()
class ECHOHEARTS_API UCraftingSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Seeds the six example recipes. Public so automation tests can seed a
    // manually constructed subsystem; idempotent.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Crafting")
    void SeedRecipes();

    // Adds shed/gifted/harvested resources to the inventory. Star tiers are
    // recomputed from each stack's lifetime points.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Crafting")
    void AddResources(const TArray<FResourceStack>& Stacks);

    // Recipes whose ingredients are fully satisfied, including star tiers.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Crafting")
    TArray<FRecipe> GetAvailableRecipes() const;

    // Crafts a recipe: validates ingredients (quantity + star tier), consumes
    // them, and returns the product ID. Nothing here harms an EcoKin.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Crafting")
    FEcoResult TryCraft(FName RecipeId, FName& OutProductId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Crafting")
    TArray<FResourceStack> GetInventory() const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Crafting")
    TArray<FRecipe> GetAllRecipes() const { return Recipes; }

private:
    bool HasIngredients(const FRecipe& Recipe) const;
    void ConsumeIngredients(const FRecipe& Recipe);

    UPROPERTY()
    TMap<FName, FResourceStack> Inventory;

    UPROPERTY()
    TArray<FRecipe> Recipes;

    bool bSeeded = false;
};
