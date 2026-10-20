// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Summoning/SummoningSubsystem.h"
#include "AnimaLink/AnimaLinkCombatSubsystem.h"
#include "EcoKin/KindlingProgressionSubsystem.h"
#include "Tamer/FrequencyTamerCharacter.h"
#include "Companion/EcoKinCompanion.h"
#include "Companion/EcoKinAIController.h"
#include "Game/EchoheartsGameMode.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

bool USummoningSubsystem::RegisterBond(int32 EcoKinID, AActor* CompanionActor)
{
    if (EcoKinID <= 0 || Roster.Contains(EcoKinID))
    {
        return false;
    }
    FBondedPartner Partner;
    Partner.EcoKinID = EcoKinID;
    Partner.CompanionActor = CompanionActor;
    Roster.Add(EcoKinID, Partner);
    return true;
}

bool USummoningSubsystem::RemoveBond(int32 EcoKinID)
{
    if (!Roster.Contains(EcoKinID))
    {
        return false;
    }
    // Cannot unbind the currently deployed partner — return them first.
    if (DeployedEcoKinID == EcoKinID)
    {
        return false;
    }
    Roster.Remove(EcoKinID);
    return true;
}

FSummonResult USummoningSubsystem::RequestSummon(int32 EcoKinID)
{
    // Ceremonies are never interrupted — not by combat, not by summoning.
    if (IsCeremonyLocked())
    {
        return FSummonResult::Refused(ESummonRefusalReason::CeremonyLocked);
    }

    FBondedPartner* Partner = Roster.Find(EcoKinID);
    if (!Partner)
    {
        // Canon guard: only a Bound partner can be called. There is no
        // summoning of the unbonded — that would be capture by another name.
        return FSummonResult::Refused(ESummonRefusalReason::NotBonded);
    }

    if (DeployedEcoKinID == EcoKinID && Partner->bDeployed)
    {
        return FSummonResult::Refused(ESummonRefusalReason::AlreadyPresent);
    }

    AFrequencyTamerCharacter* Tamer = GetTamer();
    if (!Tamer)
    {
        return FSummonResult::Refused(ESummonRefusalReason::LinkCannotBear);
    }

    // A partner who withdrew to heal cannot be called back into the field.
    // Partners are never spent; they rest until ready.
    AActor* CompanionActor = Partner->CompanionActor.Get();
    if (AEcoKinCompanion* Companion = Cast<AEcoKinCompanion>(CompanionActor))
    {
        if (Companion->bWithdrawn)
        {
            return FSummonResult::Refused(ESummonRefusalReason::Recovering);
        }
    }

    // Low trust goes unanswered. The call is heard; the partner chooses not
    // to come. Strengthen the bond — there is no override.
    const float Trust = GetTrustPercent(EcoKinID);
    if (Trust < MinTrustToAnswer)
    {
        return FSummonResult::Refused(ESummonRefusalReason::Unanswered);
    }

    // The Eco-Device spends charge to open the call.
    if (Tamer->Aegis.Charge < DeviceChargeCost)
    {
        return FSummonResult::Refused(ESummonRefusalReason::InsufficientCharge);
    }

    // The Anima-Link bears the summon. If the bond cannot bear the strain,
    // the call fails closed — never a silent fracture.
    const float StrainCost = ComputeStrainCost(Tamer, CompanionActor);
    UAnimaLinkCombatSubsystem* Combat = GetGameInstance()
        ? GetGameInstance()->GetSubsystem<UAnimaLinkCombatSubsystem>()
        : nullptr;
    if (!Combat)
    {
        return FSummonResult::Refused(ESummonRefusalReason::LinkCannotBear);
    }
    if (!Combat->ResolveTamerCommand(Tamer->AnimaLink, StrainCost).bSucceeded)
    {
        return FSummonResult::Refused(ESummonRefusalReason::LinkCannotBear);
    }

    // The call is answered. Only one partner deployed at a time — the
    // previous partner returns to rest first.
    if (DeployedEcoKinID != 0 && DeployedEcoKinID != EcoKinID)
    {
        RequestReturn();
    }

    Tamer->Aegis.Charge = FMath::Max(0.0f, Tamer->Aegis.Charge - DeviceChargeCost);
    Tamer->Aegis.Clamp();

    Partner->bDeployed = true;
    DeployedEcoKinID = EcoKinID;
    Tamer->ActiveCompanion = CompanionActor;

    if (AEcoKinCompanion* Companion = Cast<AEcoKinCompanion>(CompanionActor))
    {
        Companion->SetBondedTamer(Tamer);
        if (AEcoKinAIController* AI = Cast<AEcoKinAIController>(Companion->GetController()))
        {
            const float LinkStrain = Tamer->AnimaLink ? Tamer->AnimaLink->LinkState.LinkStrain : 0.0f;
            AI->RequestStance(ECompanionStance::Follow, Trust, LinkStrain);
        }
    }

    return FSummonResult::Ok(StrainCost);
}

FSummonResult USummoningSubsystem::RequestReturn()
{
    if (DeployedEcoKinID == 0)
    {
        return FSummonResult::Refused(ESummonRefusalReason::AlreadyPresent);
    }

    if (IsCeremonyLocked())
    {
        return FSummonResult::Refused(ESummonRefusalReason::CeremonyLocked);
    }

    FBondedPartner* Partner = Roster.Find(DeployedEcoKinID);
    AFrequencyTamerCharacter* Tamer = GetTamer();

    if (Partner)
    {
        Partner->bDeployed = false;
        if (AEcoKinCompanion* Companion = Cast<AEcoKinCompanion>(Partner->CompanionActor.Get()))
        {
            // The partner disengages and rests where it stands. Returning is
            // free — rest is part of the relationship, not a transaction.
            if (AEcoKinAIController* AI = Cast<AEcoKinAIController>(Companion->GetController()))
            {
                AI->RequestStance(ECompanionStance::Retreat, 100.0f, 0.0f);
            }
        }
    }

    if (Tamer)
    {
        Tamer->ActiveCompanion = nullptr;
    }
    DeployedEcoKinID = 0;
    return FSummonResult::Ok(0.0f);
}

TArray<int32> USummoningSubsystem::GetBondedIDs() const
{
    TArray<int32> IDs;
    Roster.GenerateKeyArray(IDs);
    return IDs;
}

float USummoningSubsystem::ComputeStrainCost(AActor* Tamer, AActor* Companion) const
{
    // Calling across distance costs more — the bond stretches. Close calls
    // are cheap; calling a partner from across a region is dear.
    float Cost = BaseStrainCost;
    if (Tamer && Companion)
    {
        const float Distance = FVector::Dist(Tamer->GetActorLocation(), Companion->GetActorLocation());
        Cost += FMath::Max(0.0f, (Distance - 1000.0f) / 1000.0f) * 2.0f;
    }
    return FMath::Min(MaxStrainCost, Cost);
}

float USummoningSubsystem::GetTrustPercent(int32 EcoKinID) const
{
    if (const UGameInstance* GI = GetGameInstance())
    {
        if (const UKindlingProgressionSubsystem* Kindling = GI->GetSubsystem<UKindlingProgressionSubsystem>())
        {
            FKindlingState State;
            if (Kindling->TryGetKindling(EcoKinID, State))
            {
                return State.TrustPercent;
            }
        }
    }
    // No recorded trust yet — a fresh bond answers tentatively.
    return MinTrustToAnswer;
}

AFrequencyTamerCharacter* USummoningSubsystem::GetTamer() const
{
    return Cast<AFrequencyTamerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

bool USummoningSubsystem::IsCeremonyLocked() const
{
    if (AGameModeBase* GM = UGameplayStatics::GetGameMode(this))
    {
        if (const AEchoheartsGameMode* EGM = Cast<AEchoheartsGameMode>(GM))
        {
            return EGM->CurrentPhase == EGamePhase::Ceremony;
        }
    }
    return false;
}
