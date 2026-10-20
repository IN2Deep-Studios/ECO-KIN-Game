// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#if WITH_DEV_AUTOMATION_TESTS

#include "Release/ReleaseHarvestTypes.h"
#include "Release/EcoKinReleaseSubsystem.h"
#include "Crafting/CraftingTypes.h"
#include "Crafting/CraftingSubsystem.h"
#include "Mutation/MutationTypes.h"
#include "Mutation/MutationDirector.h"
#include "EcoKin/EcoKinDexSubsystem.h"
#include "Misc/AutomationTest.h"

// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-14).

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FStarTierThresholdTest,
    "Echohearts.Contracts.Release.StarTierThresholds",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FStarTierThresholdTest::RunTest(const FString& Parameters)
{
    using T = EResourceStarTier;
    // Tay's spec: start at Yellow; a star every 100 lifetime points;
    // Yellow -> Bronze -> Silver -> Gold -> Platinum -> Purple -> Red Star.
    TestEqual(TEXT("0 -> Yellow"), GetStarTierForLifetimePoints(0), T::Yellow);
    TestEqual(TEXT("99 -> Yellow"), GetStarTierForLifetimePoints(99), T::Yellow);
    TestEqual(TEXT("100 -> Bronze"), GetStarTierForLifetimePoints(100), T::Bronze);
    TestEqual(TEXT("200 -> Silver"), GetStarTierForLifetimePoints(200), T::Silver);
    TestEqual(TEXT("300 -> Gold"), GetStarTierForLifetimePoints(300), T::Gold);
    TestEqual(TEXT("400 -> Platinum"), GetStarTierForLifetimePoints(400), T::Platinum);
    TestEqual(TEXT("500 -> Purple"), GetStarTierForLifetimePoints(500), T::Purple);
    TestEqual(TEXT("600 -> RedStar"), GetStarTierForLifetimePoints(600), T::RedStar);
    TestEqual(TEXT("10000 caps at RedStar"), GetStarTierForLifetimePoints(10000), T::RedStar);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FReleaseWillingnessTest,
    "Echohearts.Contracts.Release.WillingnessGate",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FReleaseWillingnessTest::RunTest(const FString& Parameters)
{
    UEcoKinReleaseSubsystem* Release = NewObject<UEcoKinReleaseSubsystem>();

    FEcoKinIdentity Identity;
    Identity.EcoKinID = 273;
    Identity.Name = FName(TEXT("Plumefane"));
    Identity.Order = EEcoKinOrder::Zoa;
    Identity.PrimaryEssence = EEcoKinEssence::Pyre;

    // Unwilling release: refused, empty yield. RELEASE IS NOT SACRIFICE.
    FReleaseYield RefusedYield;
    const FEcoResult Refused = Release->ReleaseEcoKin(Identity, false, RefusedYield);
    TestFalse(TEXT("Unwilling release fails"), Refused.bSucceeded);
    TestEqual(TEXT("Unwilling release yields nothing"), RefusedYield.Stacks.Num(), 0);

    // Willing release: succeeds with order-appropriate resources.
    FReleaseYield Yield;
    const FEcoResult Granted = Release->ReleaseEcoKin(Identity, true, Yield);
    TestTrue(TEXT("Willing release succeeds"), Granted.bSucceeded);
    TestTrue(TEXT("Willing release yields resources"), Yield.Stacks.Num() > 0);
    for (const FResourceStack& Stack : Yield.Stacks)
    {
        TestTrue(TEXT("Zoa release yields Zoa resources"),
            Stack.ResourceId.ToString().StartsWith(TEXT("Zoa.")));
        TestFalse(TEXT("Yield has a display name"), Stack.DisplayName.IsEmpty());
        TestTrue(TEXT("Yield has a quantity"), Stack.Quantity > 0);
    }

    // Flora identity yields plant resources.
    FEcoKinIdentity FloraIdentity = Identity;
    FloraIdentity.Order = EEcoKinOrder::Flora;
    FReleaseYield FloraYield;
    Release->ReleaseEcoKin(FloraIdentity, true, FloraYield);
    bool bSawPetal = false;
    for (const FResourceStack& Stack : FloraYield.Stacks)
    {
        if (Stack.ResourceId == FName(TEXT("Flora.FallenPetal")))
        {
            bSawPetal = true;
        }
    }
    TestTrue(TEXT("Flora release yields fallen petals"), bSawPetal);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FHarvestRespawnTest,
    "Echohearts.Contracts.Release.HarvestRespawn",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FHarvestRespawnTest::RunTest(const FString& Parameters)
{
    UEcoKinReleaseSubsystem* Release = NewObject<UEcoKinReleaseSubsystem>();

    FHarvestNode Node;
    Node.NodeId = FName(TEXT("Test.BloomGrove"));
    Node.DisplayName = FText::FromString(TEXT("Test Bloom Grove"));
    Node.ResourceId = FName(TEXT("Flora.FallenPetal"));
    Node.YieldPerHarvest = 3;
    Node.RespawnSeconds = 3600.0f;
    Node.PointsPerUnit = 1.0f;
    TestTrue(TEXT("Node registers"), Release->RegisterFloraNode(Node).bSucceeded);

    FReleaseYield First;
    TestTrue(TEXT("First harvest succeeds"), Release->HarvestNode(Node.NodeId, First).bSucceeded);
    TestEqual(TEXT("First harvest yields one stack"), First.Stacks.Num(), 1);
    TestEqual(TEXT("First harvest quantity"), First.Stacks[0].Quantity, 3);

    FReleaseYield Second;
    const FEcoResult Regrowing = Release->HarvestNode(Node.NodeId, Second);
    TestFalse(TEXT("Immediate re-harvest refused while regrowing"), Regrowing.bSucceeded);
    TestEqual(TEXT("Refused harvest yields nothing"), Second.Stacks.Num(), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FCraftingStarTierGateTest,
    "Echohearts.Contracts.Crafting.StarTierGating",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FCraftingStarTierGateTest::RunTest(const FString& Parameters)
{
    UCraftingSubsystem* Craft = NewObject<UCraftingSubsystem>();
    Craft->SeedRecipes();
    TestEqual(TEXT("Six seed recipes"), Craft->GetAllRecipes().Num(), 6);

    auto MakeStack = [](const FString& Id, int32 Qty, int32 Lifetime) -> FResourceStack
    {
        FResourceStack S;
        S.ResourceId = FName(*Id);
        S.DisplayName = FText::FromString(Id);
        S.Quantity = Qty;
        S.LifetimePoints = Lifetime;
        S.StarTier = GetStarTierForLifetimePoints(Lifetime);
        return S;
    };

    // Low-tier sap: recipe needs Silver sap, so nothing gated on sap is available.
    Craft->AddResources({ MakeStack(TEXT("Flora.OfferedSap"), 10, 50) });
    bool bSawAmplifier = false;
    for (const FRecipe& R : Craft->GetAvailableRecipes())
    {
        if (R.RecipeId == FName(TEXT("Aegis.ResonanceAmplifier")))
        {
            bSawAmplifier = true;
        }
    }
    TestFalse(TEXT("Silver-gated recipe hidden at Yellow tier"), bSawAmplifier);

    // Earn Silver standing on sap and crystal: the amplifier unlocks.
    Craft->AddResources({
        MakeStack(TEXT("Flora.OfferedSap"), 0, 250),
        MakeStack(TEXT("Proto.LoosedCrystalShard"), 2, 150),
    });
    bSawAmplifier = false;
    for (const FRecipe& R : Craft->GetAvailableRecipes())
    {
        if (R.RecipeId == FName(TEXT("Aegis.ResonanceAmplifier")))
        {
            bSawAmplifier = true;
        }
    }
    TestTrue(TEXT("Recipe unlocks once star tiers are met"), bSawAmplifier);

    FName ProductId;
    const FEcoResult Crafted = Craft->TryCraft(FName(TEXT("Aegis.ResonanceAmplifier")), ProductId);
    TestTrue(TEXT("Craft succeeds"), Crafted.bSucceeded);
    TestEqual(TEXT("Craft returns product ID"),
        ProductId, FName(TEXT("AegisMod.ResonanceAmplifier")));

    int32 SapLeft = 0;
    for (const FResourceStack& S : Craft->GetInventory())
    {
        if (S.ResourceId == FName(TEXT("Flora.OfferedSap")))
        {
            SapLeft = S.Quantity;
        }
    }
    TestEqual(TEXT("Ingredients consumed (10 - 4)"), SapLeft, 6);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FMutationFormAttachTest,
    "Echohearts.Contracts.Mutation.AttachesToBaseIdentity",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMutationFormAttachTest::RunTest(const FString& Parameters)
{
    UEcoKinDexSubsystem* Dex = NewObject<UEcoKinDexSubsystem>();
    UMutationDirector* Director = NewObject<UMutationDirector>();
    Director->SetDexSubsystemOverride(Dex);

    FEcoKinIdentity Identity;
    Identity.EcoKinID = 300;
    Identity.Name = FName(TEXT("Frogggleaf"));
    Identity.Order = EEcoKinOrder::Flora;
    Identity.PrimaryEssence = EEcoKinEssence::Flora;
    Identity.EchoprintHash = TEXT("test-echoprint");
    TestTrue(TEXT("Identity registers"), Dex->RegisterIdentity(Identity).bSucceeded);

    FMutationTrigger Trigger;
    Trigger.TriggerKind = EMutationTriggerKind::ResonanceStrain;
    Trigger.TargetEcoKinID = 300;
    Trigger.CatalystId = FName(TEXT("Test.ResonanceShard"));
    Trigger.MutationNodeIDs.Add(FName(TEXT("Test.Node.VerdantSurge")));

    FEcoformRecord OutForm;
    const FEcoResult Result = Director->RequestMutation(Trigger, OutForm);
    TestTrue(TEXT("Mutation succeeds"), Result.bSucceeded);
    TestEqual(TEXT("Form kind is Mutation"), OutForm.Kind, EEcoformKind::Mutation);
    TestEqual(TEXT("Form attaches to base identity"), OutForm.BaseEcoKinID, 300);
    TestEqual(TEXT("Mutation nodes carried onto form"), OutForm.MutationNodeIDs.Num(), 1);

    const TArray<FEcoformRecord> Forms = Dex->GetFormsFor(300);
    TestEqual(TEXT("Forms Registry holds the mutation"), Forms.Num(), 1);

    // Canon guard: Nature allows conditional Mutations only.
    Director->RegisterNatureIdentity(300);
    FMutationTrigger Unconditional = Trigger;
    Unconditional.bConditionalOnly = false;
    FEcoformRecord RejectedForm;
    const FEcoResult Denied = Director->RequestMutation(Unconditional, RejectedForm);
    TestFalse(TEXT("Unconditional Nature mutation refused"), Denied.bSucceeded);

    FMutationTrigger Conditional = Trigger;
    Conditional.bConditionalOnly = true;
    FEcoformRecord AcceptedForm;
    const FEcoResult Allowed = Director->RequestMutation(Conditional, AcceptedForm);
    TestTrue(TEXT("Conditional Nature mutation allowed"), Allowed.bSucceeded);
    return true;
}

#endif
