// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "AnimaLink/AnimaLinkComponent.h"

UAnimaLinkComponent::UAnimaLinkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

FEcoResult UAnimaLinkComponent::BeginLink()
{
    if (LinkState.bLinkActive)
    {
        return FEcoResult::Fail(
            FName(TEXT("Echohearts.AnimaLink.AlreadyActive")),
            FText::FromString(TEXT("Anima-Link is already active.")));
    }

    LinkState.bLinkActive = true;
    LinkState.LinkStrain = 0.0f;
    return FEcoResult::Ok();
}

FEcoResult UAnimaLinkComponent::EndLink()
{
    LinkState.bLinkActive = false;
    LinkState.LinkStrain = 0.0f;
    return FEcoResult::Ok();
}

float UAnimaLinkComponent::NotifyEcoKinStrain(float StrainAmount)
{
    if (!LinkState.bLinkActive || StrainAmount <= 0.0f)
    {
        return 0.0f;
    }

    LinkState.ApplyStrain(StrainAmount);
    // Bidirectional feedback: a fraction lands on tamer stamina.
    return StrainAmount * LinkState.TamerStaminaFeedback;
}

float UAnimaLinkComponent::NotifyTamerOverexertion(float ExertionAmount)
{
    if (!LinkState.bLinkActive || ExertionAmount <= 0.0f)
    {
        return 0.0f;
    }

    LinkState.ApplyStrain(ExertionAmount);
    // Returns the strain now carried by the link so callers can surface warnings.
    return LinkState.LinkStrain;
}

bool UAnimaLinkComponent::TryPayTransformationCost(float StrainCost)
{
    if (!LinkState.bLinkActive || StrainCost < 0.0f)
    {
        return false;
    }

    if (LinkState.LinkStrain + StrainCost > 100.0f)
    {
        return false;
    }

    LinkState.ApplyStrain(StrainCost);
    return true;
}
