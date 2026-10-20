// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Crafting/CraftingSubsystem.h"
#include "Echohearts.h"

void UCraftingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    SeedRecipes();
}

void UCraftingSubsystem::SeedRecipes()
{
    if (bSeeded)
    {
        return;
    }
    bSeeded = true;
    Recipes.Empty();

    auto MakeRecipe = [&](const FString& Id, const FString& Name,
        EProductKind Kind, const FString& ProductId, const FString& Blurb,
        TArray<FIngredient> Ingredients) -> FRecipe
    {
        FRecipe R;
        R.RecipeId = FName(*Id);
        R.DisplayName = FText::FromString(Name);
        R.ProductKind = Kind;
        R.ProductId = FName(*ProductId);
        R.ProductBlurb = FText::FromString(Blurb);
        R.Ingredients = MoveTemp(Ingredients);
        return R;
    };

    auto Ing = [](const FString& ResourceId, int32 Qty, EResourceStarTier Tier) -> FIngredient
    {
        FIngredient I;
        I.ResourceId = FName(*ResourceId);
        I.Quantity = Qty;
        I.MinStarTier = Tier;
        return I;
    };

    // 1. A.E.G.I.S. module: amplifies resonance readings in the field.
    Recipes.Add(MakeRecipe(
        TEXT("Aegis.ResonanceAmplifier"), TEXT("A.E.G.I.S. Resonance Amplifier"),
        EProductKind::AegisModule, TEXT("AegisMod.ResonanceAmplifier"),
        TEXT("Tuned sap and crystal focus that steadies A.E.G.I.S. resonance readings."),
        { Ing(TEXT("Flora.OfferedSap"), 4, EResourceStarTier::Silver),
          Ing(TEXT("Proto.LoosedCrystalShard"), 2, EResourceStarTier::Bronze) }));

    // 2. A.E.G.I.S. module: long-range creature movement scanner.
    Recipes.Add(MakeRecipe(
        TEXT("Aegis.FieldScanner"), TEXT("A.E.G.I.S. Field Scanner"),
        EProductKind::AegisModule, TEXT("AegisMod.FieldScanner"),
        TEXT("Feather-vane sensors that read the wind for distant movement."),
        { Ing(TEXT("Zoa.FallenFeather"), 3, EResourceStarTier::Bronze),
          Ing(TEXT("Myko.EssenceTrace"), 2, EResourceStarTier::Silver) }));

    // 3. Sanctuary structure: a greenhouse dome for restoration farming.
    Recipes.Add(MakeRecipe(
        TEXT("Sanctuary.GreenhouseDome"), TEXT("Sanctuary Greenhouse Dome"),
        EProductKind::SanctuaryStructure, TEXT("Sanctuary.GreenhouseDome"),
        TEXT("A living dome of petal-lattice that shelters young restoration crops."),
        { Ing(TEXT("Flora.FallenPetal"), 8, EResourceStarTier::Bronze),
          Ing(TEXT("Flora.OfferedSap"), 4, EResourceStarTier::Bronze) }));

    // 4. Sanctuary structure: beacon that steadies local wilds.
    Recipes.Add(MakeRecipe(
        TEXT("Sanctuary.RestorationBeacon"), TEXT("Sanctuary Restoration Beacon"),
        EProductKind::SanctuaryStructure, TEXT("Sanctuary.RestorationBeacon"),
        TEXT("Bio-essence core wrapped in molted scale that calms the surrounding wilds."),
        { Ing(TEXT("Monera.ShedBioEssence"), 2, EResourceStarTier::Silver),
          Ing(TEXT("Zoa.MoltedScale"), 4, EResourceStarTier::Bronze) }));

    // 5. Tamer gear: protective trail cloak (utility wear, never a weapon).
    Recipes.Add(MakeRecipe(
        TEXT("Tamer.TrailCloak"), TEXT("Tamer's Trail Cloak"),
        EProductKind::TamerGear, TEXT("TamerGear.TrailCloak"),
        TEXT("Layered shed hide that turns thorns and weather."),
        { Ing(TEXT("Zoa.ShedHide"), 5, EResourceStarTier::Silver) }));

    // 6. Consumable: a soothing balm for post-link recovery.
    Recipes.Add(MakeRecipe(
        TEXT("Consumable.SoothingBalm"), TEXT("Soothing Balm"),
        EProductKind::Consumable, TEXT("Consumable.SoothingBalm"),
        TEXT("Spore-and-sap salve that eases Anima-Link strain after hard battles."),
        { Ing(TEXT("Flora.DriftedSpore"), 3, EResourceStarTier::Yellow),
          Ing(TEXT("Flora.OfferedSap"), 2, EResourceStarTier::Yellow) }));
}

void UCraftingSubsystem::AddResources(const TArray<FResourceStack>& Stacks)
{
    for (const FResourceStack& Incoming : Stacks)
    {
        if (Incoming.ResourceId.IsNone())
        {
            continue;
        }
        FResourceStack& Slot = Inventory.FindOrAdd(Incoming.ResourceId);
        Slot.ResourceId = Incoming.ResourceId;
        Slot.DisplayName = Incoming.DisplayName;
        Slot.SourceOrder = Incoming.SourceOrder;
        Slot.SourceEssence = Incoming.SourceEssence;
        // Lifetime standing can grow without new units (research, milestones).
        if (Incoming.Quantity > 0)
        {
            Slot.Quantity += Incoming.Quantity;
        }
        Slot.LifetimePoints = FMath::Max(Slot.LifetimePoints, Incoming.LifetimePoints);
        Slot.StarTier = GetStarTierForLifetimePoints(Slot.LifetimePoints);
    }
}

TArray<FRecipe> UCraftingSubsystem::GetAvailableRecipes() const
{
    TArray<FRecipe> Available;
    for (const FRecipe& Recipe : Recipes)
    {
        if (HasIngredients(Recipe))
        {
            Available.Add(Recipe);
        }
    }
    return Available;
}

FEcoResult UCraftingSubsystem::TryCraft(FName RecipeId, FName& OutProductId)
{
    OutProductId = NAME_None;

    const FRecipe* Recipe = Recipes.FindByPredicate(
        [&](const FRecipe& R) { return R.RecipeId == RecipeId; });
    if (!Recipe)
    {
        return FEcoResult::Fail(
            FName(TEXT("Crafting.UnknownRecipe")),
            FText::FromString(TEXT("Unknown recipe.")));
    }

    if (!HasIngredients(*Recipe))
    {
        return FEcoResult::Fail(
            FName(TEXT("Crafting.MissingIngredients")),
            FText::FromString(TEXT("Ingredients missing or star tier too low.")));
    }

    ConsumeIngredients(*Recipe);
    OutProductId = Recipe->ProductId;
    return FEcoResult::Ok();
}

TArray<FResourceStack> UCraftingSubsystem::GetInventory() const
{
    TArray<FResourceStack> Out;
    Inventory.GenerateValueArray(Out);
    return Out;
}

bool UCraftingSubsystem::HasIngredients(const FRecipe& Recipe) const
{
    for (const FIngredient& Need : Recipe.Ingredients)
    {
        const FResourceStack* Have = Inventory.Find(Need.ResourceId);
        if (!Have)
        {
            return false;
        }
        if (Have->Quantity < Need.Quantity)
        {
            return false;
        }
        if (static_cast<int32>(Have->StarTier) < static_cast<int32>(Need.MinStarTier))
        {
            return false;
        }
    }
    return true;
}

void UCraftingSubsystem::ConsumeIngredients(const FRecipe& Recipe)
{
    for (const FIngredient& Need : Recipe.Ingredients)
    {
        if (FResourceStack* Have = Inventory.Find(Need.ResourceId))
        {
            Have->Quantity = FMath::Max(0, Have->Quantity - Need.Quantity);
        }
    }
}
