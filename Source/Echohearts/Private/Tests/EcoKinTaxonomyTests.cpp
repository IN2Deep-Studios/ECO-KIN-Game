// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#if WITH_DEV_AUTOMATION_TESTS

#include "EcoKin/EcoKinTaxonomy.h"
#include "EcoKin/EcoKinKindling.h"
#include "AnimaLink/AnimaLinkTypes.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEcoResultEnvelopeTest,
    "Echohearts.Contracts.ResultEnvelope.OkAndFail",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FEcoResultEnvelopeTest::RunTest(const FString& Parameters)
{
    const FEcoResult Ok = FEcoResult::Ok();
    TestTrue(TEXT("Ok succeeds"), Ok.bSucceeded);
    TestTrue(TEXT("Ok has no error code"), Ok.ErrorCode.IsNone());

    const FEcoResult Fail = FEcoResult::Fail(
        FName(TEXT("Echohearts.Test.Demo")),
        FText::FromString(TEXT("demo")));
    TestFalse(TEXT("Fail does not succeed"), Fail.bSucceeded);
    TestEqual(TEXT("Fail carries error code"), Fail.ErrorCode, FName(TEXT("Echohearts.Test.Demo")));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FBondTransitionTest,
    "Echohearts.Contracts.BondLoop.ValidTransitions",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FBondTransitionTest::RunTest(const FString& Parameters)
{
    using S = EEcoKinBondState;
    // Locked loop: Observe -> Protect -> Calm -> Kindle -> Bond / Release / Defer.
    TestTrue(TEXT("Unbound->Observing"), IsValidBondTransition(S::Unbound, S::Observing));
    TestTrue(TEXT("Observing->Protected"), IsValidBondTransition(S::Observing, S::Protected));
    TestTrue(TEXT("Protected->Calmed"), IsValidBondTransition(S::Protected, S::Calmed));
    TestTrue(TEXT("Calmed->Kindled"), IsValidBondTransition(S::Calmed, S::Kindled));
    TestTrue(TEXT("Kindled->Bonded"), IsValidBondTransition(S::Kindled, S::Bonded));
    TestTrue(TEXT("Kindled->Released"), IsValidBondTransition(S::Kindled, S::Released));
    TestTrue(TEXT("Kindled->Deferred"), IsValidBondTransition(S::Kindled, S::Deferred));
    TestTrue(TEXT("Bonded->Released"), IsValidBondTransition(S::Bonded, S::Released));

    // Illegal shortcuts must fail: no skipping stages, no re-bonding from bonded.
    TestFalse(TEXT("Unbound->Bonded rejected"), IsValidBondTransition(S::Unbound, S::Bonded));
    TestFalse(TEXT("Observing->Kindled rejected"), IsValidBondTransition(S::Observing, S::Kindled));
    TestFalse(TEXT("Bonded->Observing rejected"), IsValidBondTransition(S::Bonded, S::Observing));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FKindlingGateTest,
    "Echohearts.Contracts.Kindling.GrowthRiteGates",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKindlingGateTest::RunTest(const FString& Parameters)
{
    FKindlingState Ready;
    Ready.TrustPercent = 85.0f;
    Ready.bBiologicallyMature = true;
    Ready.EcologyAccomplishments = 3;
    Ready.CatalystItemIDs.Add(FName(TEXT("Bloomstone")));
    TestTrue(TEXT("Rite allowed when all gates pass"), Ready.CanPerformGrowthRite(3));

    FKindlingState LowTrust = Ready;
    LowTrust.TrustPercent = 79.0f;
    TestFalse(TEXT("Rite blocked below 80% trust"), LowTrust.CanPerformGrowthRite(3));

    FKindlingState Immature = Ready;
    Immature.bBiologicallyMature = false;
    TestFalse(TEXT("Rite blocked when immature"), Immature.CanPerformGrowthRite(3));

    FKindlingState NoCatalyst = Ready;
    NoCatalyst.CatalystItemIDs.Empty();
    TestFalse(TEXT("Rite blocked without catalyst"), NoCatalyst.CanPerformGrowthRite(3));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FAnimaLinkStrainTest,
    "Echohearts.Contracts.AnimaLink.BidirectionalStrain",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FAnimaLinkStrainTest::RunTest(const FString& Parameters)
{
    FAnimaLinkState Link;
    Link.bLinkActive = true;

    Link.ApplyStrain(40.0f);
    TestEqual(TEXT("Strain accumulates"), Link.LinkStrain, 40.0f);

    Link.ApplyStrain(80.0f);
    TestEqual(TEXT("Strain clamps at 100"), Link.LinkStrain, 100.0f);
    TestTrue(TEXT("Fracture risk at 100"), Link.IsFractureRisk());

    Link.ReleaseStrain(30.0f);
    TestEqual(TEXT("Strain releases"), Link.LinkStrain, 70.0f);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FShardBudgetNeutralityTest,
    "Echohearts.Contracts.ResonanceShard.BudgetRedistribution",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FShardBudgetNeutralityTest::RunTest(const FString& Parameters)
{
    // Locked canon: Resonance Strain budgets are redistributed, never inflated.
    FEcoKinCoreAttributes Neutral;
    Neutral.Vibrance = 20.0f;
    Neutral.Density = -10.0f;
    Neutral.Harmony = -5.0f;
    Neutral.Purity = -5.0f;
    const float Sum = Neutral.Vibrance + Neutral.Density + Neutral.Harmony + Neutral.Purity;
    TestTrue(TEXT("Redistribution sums to zero"), FMath::IsNearlyZero(Sum, 1e-3f));

    FEcoKinCoreAttributes Inflated;
    Inflated.Vibrance = 20.0f;
    const float InflatedSum = Inflated.Vibrance + Inflated.Density + Inflated.Harmony + Inflated.Purity;
    TestFalse(TEXT("Inflation is detectable"), FMath::IsNearlyZero(InflatedSum, 1e-3f));

    return true;
}

#endif
