// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "AnimaLink/AnimaLinkCombatSubsystem.h"
#include "AnimaLink/AnimaLinkComponent.h"

FEcoResult UAnimaLinkCombatSubsystem::ResolveEcoKinHit(UAnimaLinkComponent* Link, float DamageAmount, float& OutTamerStaminaFeedback)
{
    OutTamerStaminaFeedback = 0.0f;

    if (!Link || !Link->LinkState.bLinkActive)
    {
        return FEcoResult::Fail(FName(TEXT("AnimaLink.Combat.NoActiveLink")),
            FText::FromString(TEXT("No active Anima-Link: combat exchange refused.")));
    }

    if (DamageAmount < 0.0f)
    {
        return FEcoResult::Fail(FName(TEXT("AnimaLink.Combat.NegativeDamage")),
            FText::FromString(TEXT("Damage amount must not be negative.")));
    }

    // Fracture guard: a hit that would max out link strain is refused rather
    // than allowed to silently fracture the bond (Shadow-fracture condition).
    if (Link->LinkState.LinkStrain + DamageAmount >= 100.0f)
    {
        return FEcoResult::Fail(FName(TEXT("AnimaLink.Combat.FractureRisk")),
            FText::FromString(TEXT("Exchange refused: it would fracture the Anima-Link. Restore the bond first.")));
    }

    // Bidirectional feedback: the partner's pain becomes the tamer's stamina cost.
    OutTamerStaminaFeedback = Link->NotifyEcoKinStrain(DamageAmount);
    return FEcoResult::Ok();
}

FEcoResult UAnimaLinkCombatSubsystem::ResolveTamerCommand(UAnimaLinkComponent* Link, float CommandStrainCost)
{
    if (!Link || !Link->LinkState.bLinkActive)
    {
        return FEcoResult::Fail(FName(TEXT("AnimaLink.Combat.NoActiveLink")),
            FText::FromString(TEXT("No active Anima-Link: command refused.")));
    }

    if (CommandStrainCost < 0.0f)
    {
        return FEcoResult::Fail(FName(TEXT("AnimaLink.Combat.NegativeCost")),
            FText::FromString(TEXT("Command strain cost must not be negative.")));
    }

    // Tamer-side overexertion strains the link in return. The command only
    // executes if the link can bear it.
    if (!Link->TryPayTransformationCost(CommandStrainCost))
    {
        return FEcoResult::Fail(FName(TEXT("AnimaLink.Combat.CommandRefused")),
            FText::FromString(TEXT("Command refused: the link cannot bear the strain. Rest first.")));
    }

    return FEcoResult::Ok();
}

bool UAnimaLinkCombatSubsystem::IsLinkFitForCombat(const UAnimaLinkComponent* Link) const
{
    return Link
        && Link->LinkState.bLinkActive
        && !Link->LinkState.IsFractureRisk();
}
