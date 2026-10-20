// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Mutation/MutationDirector.h"
#include "EcoKin/EcoKinDexSubsystem.h"
#include "Echohearts.h"

void UMutationDirector::RegisterNatureIdentity(int32 EcoKinID)
{
    if (EcoKinID > 0)
    {
        NatureIdentityIDs.Add(EcoKinID);
    }
}

UEcoKinDexSubsystem* UMutationDirector::ResolveDex() const
{
    if (DexOverride)
    {
        return DexOverride;
    }
    if (UGameInstance* GI = GetGameInstance())
    {
        return GI->GetSubsystem<UEcoKinDexSubsystem>();
    }
    return nullptr;
}

FEcoResult UMutationDirector::RequestMutation(const FMutationTrigger& Trigger, FEcoformRecord& OutForm)
{
    OutForm = FEcoformRecord();

    if (Trigger.TargetEcoKinID <= 0)
    {
        return FEcoResult::Fail(
            FName(TEXT("Mutation.InvalidTarget")),
            FText::FromString(TEXT("Mutation requires a valid target EcoKin ID.")));
    }

    UEcoKinDexSubsystem* Dex = ResolveDex();
    if (!Dex)
    {
        return FEcoResult::Fail(
            FName(TEXT("Mutation.NoDex")),
            FText::FromString(TEXT("Mutation requires the Active Dex subsystem.")));
    }

    FEcoKinIdentity Identity;
    if (!Dex->TryGetIdentity(Trigger.TargetEcoKinID, Identity))
    {
        return FEcoResult::Fail(
            FName(TEXT("Mutation.UnregisteredTarget")),
            FText::FromString(TEXT("Mutations attach only to registered EcoKin identities.")));
    }

    // CANON GUARD: Nature (Legendary Humanoid-Kin) allows conditional Mutations
    // only. An unconditional request is refused — never silently downgraded.
    if (NatureIdentityIDs.Contains(Trigger.TargetEcoKinID) && !Trigger.bConditionalOnly)
    {
        return FEcoResult::Fail(
            FName(TEXT("Mutation.Nature.UnconditionalDenied")),
            FText::FromString(TEXT("Nature permits conditional Mutations only. Unconditional mutation refused.")));
    }

    FEcoformRecord Form;
    Form.FormID = FName(*(Identity.Name.ToString() + TEXT("_Mutation")));
    Form.BaseEcoKinID = Trigger.TargetEcoKinID;
    Form.Kind = EEcoformKind::Mutation;
    Form.DisplayName = FName(*(Identity.Name.ToString() + TEXT(" Mutated")));
    Form.bPreservesIdentity = true;
    Form.MutationNodeIDs = Trigger.MutationNodeIDs;

    const FEcoResult AttachResult = Dex->AttachForm(Form);
    if (!AttachResult.bSucceeded)
    {
        return AttachResult;
    }

    OutForm = Form;
    return FEcoResult::Ok();
}
