// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Release/ReleaseHarvestTypes.h"
#include "CraftingTypes.generated.h"

// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-14).
// Part-based crafting: shed/gifted/harvested resources become A.E.G.I.S. modules,
// tamer gear, Sanctuary structures, and consumables. The METHOD is the
// ecology-first gathering loop (hunt/gather parts, craft gear); no external
// game's names, monsters, items, or fiction appear anywhere here.
// Canon lock: nothing crafted may be a weapon that harms EcoKin.

// What a recipe produces. TamerGear is protective/utility wear only.
UENUM(BlueprintType)
enum class EProductKind : uint8
{
    TamerGear          UMETA(DisplayName = "Tamer Gear"),
    AegisModule        UMETA(DisplayName = "A.E.G.I.S. Module"),
    SanctuaryStructure UMETA(DisplayName = "Sanctuary Structure"),
    Consumable         UMETA(DisplayName = "Consumable")
};

// One ingredient: a resource ID, a quantity, and the minimum star tier the
// tamer must have earned for that resource (Tay's star-tier gating).
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FIngredient
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Crafting")
    FName ResourceId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Crafting")
    int32 Quantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Crafting")
    EResourceStarTier MinStarTier = EResourceStarTier::Yellow;
};

// A crafting recipe: ingredients in, one product out.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FRecipe
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Crafting")
    FName RecipeId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Crafting")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Crafting")
    TArray<FIngredient> Ingredients;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Crafting")
    EProductKind ProductKind = EProductKind::Consumable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Crafting")
    FName ProductId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Crafting")
    FText ProductBlurb;
};
