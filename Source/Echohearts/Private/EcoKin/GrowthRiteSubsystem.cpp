// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "EcoKin/GrowthRiteSubsystem.h"
#include "EcoKin/EcoKinDexSubsystem.h"
#include "Mutation/MutationDirector.h"
#include "AnimaLink/AnimaLinkComponent.h"
#include "Engine/GameInstance.h"

FEcoResult UGrowthRiteSubsystem::RequestGrowthRite(int32 EcoKinID, FKindlingState& Kindling, UAnimaLinkComponent* Link,
    int32 RequiredAccomplishments, float RiteStrainCost, FEcoformRecord& OutForm)
{
    UEcoKinDexSubsystem* Dex = ResolveDex();
    if (!Dex)
    {
        return FEcoResult::Fail(FName(TEXT("GrowthRite.NoDexSubsystem")),
            FText::FromString(TEXT("Growth Rite refused: no Dex subsystem available.")));
    }

    // GATE 1-4: Biological Maturity, Kindling (trust %), Ecology
    // accomplishments, Catalyst items. All four are required; a rite is
    // maturation, not a purchase.
    if (!Kindling.CanPerformGrowthRite(RequiredAccomplishments))
    {
        return FEcoResult::Fail(FName(TEXT("GrowthRite.GateNotMet")),
            FText::FromString(TEXT("Growth Rite refused: maturity, trust, accomplishments, or catalyst missing.")));
    }

    if (!Link || !Link->LinkState.bLinkActive)
    {
        return FEcoResult::Fail(FName(TEXT("GrowthRite.NoActiveLink")),
            FText::FromString(TEXT("Growth Rite refused: no active Anima-Link.")));
    }

    // The transformation pays Link Strain FIRST. The link must bear the rite.
    if (!Link->TryPayTransformationCost(RiteStrainCost))
    {
        return FEcoResult::Fail(FName(TEXT("GrowthRite.LinkCannotBear")),
            FText::FromString(TEXT("Growth Rite refused: the Anima-Link cannot bear the transformation strain.")));
    }

    FEcoKinIdentity Identity;
    if (!Dex->TryGetIdentity(EcoKinID, Identity))
    {
        return FEcoResult::Fail(FName(TEXT("GrowthRite.UnknownIdentity")),
            FText::FromString(TEXT("Growth Rite refused: Eco-Kin identity is not registered.")));
    }

    // Snapshot the Echoprint BEFORE the rite. It must be identical after.
    const FString EchoprintBefore = Identity.EchoprintHash;

    // Consume one catalyst: the rite is earned AND paid for, never free.
    const FName ConsumedCatalyst = Kindling.CatalystItemIDs[0];
    Kindling.CatalystItemIDs.RemoveAt(0);

    UMutationDirector* MutationDirector = ResolveMutationDirector();

    // CANON GUARD: Nature (Legendary Humanoid-Kin) takes CONDITIONAL Mutations
    // only. Rite requests for a registered Nature identity route through the
    // Mutation Director with bConditionalOnly set; the director refuses
    // unconditional mutation every time. There is deliberately no other path.
    FEcoResult FormResult;
    const bool bIsNatureIdentity = MutationDirector && MutationDirector->IsNatureIdentity(EcoKinID);
    if (bIsNatureIdentity)
    {
        FMutationTrigger Trigger;
        Trigger.TriggerKind = EMutationTriggerKind::ResonanceStrain;
        Trigger.TargetEcoKinID = EcoKinID;
        Trigger.CatalystId = ConsumedCatalyst;
        Trigger.bConditionalOnly = true;

        FormResult = MutationDirector->RequestMutation(Trigger, OutForm);
        if (!FormResult.bSucceeded)
        {
            // The director refused. Its refusal stands; do not fall through
            // to the standard path.
            return FormResult;
        }
    }
    else
    {
        // Standard Growth Rite: attach the new Ecoform to the base identity.
        // (Form construction is gameplay-side; the Dex enforces attachment rules.)
        OutForm.BaseEcoKinID = EcoKinID;
        OutForm.Kind = EEcoformKind::GrowthRite;
        OutForm.bPreservesIdentity = true;
        FormResult = Dex->AttachForm(OutForm);
        if (!FormResult.bSucceeded)
        {
            return FormResult;
        }
    }

    // Echoprint verification: the identity must be byte-identical after the rite.
    // An approved appearance is never silently replaced.
    FEcoKinIdentity IdentityAfter;
    if (Dex->TryGetIdentity(EcoKinID, IdentityAfter) && IdentityAfter.EchoprintHash != EchoprintBefore)
    {
        return FEcoResult::Fail(FName(TEXT("GrowthRite.EchoprintChanged")),
            FText::FromString(TEXT("Growth Rite aborted: Echoprint changed during the rite. This must never happen.")));
    }

    // NOTE: there is intentionally no sacrifice/consume/donor path in this
    // subsystem. Willing Release is handled by the Release subsystem and is
    // never a rite cost. No such function may be added here.
    return FEcoResult::Ok();
}

UEcoKinDexSubsystem* UGrowthRiteSubsystem::ResolveDex() const
{
    if (DexOverride)
    {
        return DexOverride;
    }
    if (const UGameInstance* GI = GetGameInstance())
    {
        return GI->GetSubsystem<UEcoKinDexSubsystem>();
    }
    return nullptr;
}

UMutationDirector* UGrowthRiteSubsystem::ResolveMutationDirector() const
{
    if (MutationOverride)
    {
        return MutationOverride;
    }
    if (const UGameInstance* GI = GetGameInstance())
    {
        return GI->GetSubsystem<UMutationDirector>();
    }
    return nullptr;
}
