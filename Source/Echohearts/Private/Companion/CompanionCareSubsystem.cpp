// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Companion/CompanionCareSubsystem.h"

void UCompanionCareSubsystem::RegisterWorker(int32 EcoKinID)
{
    if (EcoKinID <= 0 || Workers.Contains(EcoKinID))
    {
        return;
    }
    Workers.Add(EcoKinID);
    if (!Conditions.Contains(EcoKinID))
    {
        Conditions.Add(EcoKinID, FCompanionCondition());
    }

    if (!bTimerRunning && GetWorld())
    {
        FTimerDelegate TickDelegate = FTimerDelegate::CreateUObject(this, &UCompanionCareSubsystem::WorkTick);
        GetWorld()->GetTimerManager().SetTimer(WorkTimerHandle, TickDelegate, 1.0f, true);
        bTimerRunning = true;
    }
}

void UCompanionCareSubsystem::UnregisterWorker(int32 EcoKinID)
{
    Workers.Remove(EcoKinID);
    if (Workers.IsEmpty() && bTimerRunning && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(WorkTimerHandle);
        bTimerRunning = false;
    }
}

bool UCompanionCareSubsystem::RestCompanion(int32 EcoKinID, float HoursRested, float RecoveryPerHour)
{
    FCompanionCondition* Cond = Conditions.Find(EcoKinID);
    if (!Cond || HoursRested <= 0.0f)
    {
        return false;
    }
    Cond->Energy = FMath::Min(100.0f, Cond->Energy + HoursRested * RecoveryPerHour);
    OnEnergyChanged.Broadcast(EcoKinID, Cond->Energy);
    if (Cond->Energy > 0.0f)
    {
        ClearAilment(EcoKinID, ECompanionAilment::Weary);
    }
    return true;
}

bool UCompanionCareSubsystem::PlayCompanion(int32 EcoKinID, float MinutesPlayed)
{
    FCompanionCondition* Cond = Conditions.Find(EcoKinID);
    if (!Cond || MinutesPlayed <= 0.0f)
    {
        return false;
    }
    // Play restores faster than idle rest and lifts spirits: the game can
    // grant a small Kindling bump on this broadcast.
    Cond->Energy = FMath::Min(100.0f, Cond->Energy + MinutesPlayed * 1.5f);
    OnEnergyChanged.Broadcast(EcoKinID, Cond->Energy);
    if (Cond->Energy > 0.0f)
    {
        ClearAilment(EcoKinID, ECompanionAilment::Weary);
    }
    return true;
}

bool UCompanionCareSubsystem::TreatAilment(int32 EcoKinID, ECompanionAilment Ailment, ECareSource Source)
{
    FCompanionCondition* Cond = Conditions.Find(EcoKinID);
    if (!Cond || !Cond->HasAilment(Ailment))
    {
        return false;
    }

    // Right care for the ailment. Play doesn't cure sickness; rest alone
    // doesn't either — Ailing needs the Mender, remedies, or meals.
    bool bSuited = false;
    switch (Ailment)
    {
    case ECompanionAilment::Ailing:
        bSuited = (Source == ECareSource::Mender || Source == ECareSource::Remedy || Source == ECareSource::Meal);
        break;
    case ECompanionAilment::Shaken:
        bSuited = (Source == ECareSource::Counsel || Source == ECareSource::Springs ||
                   Source == ECareSource::Campfire || Source == ECareSource::Play);
        break;
    case ECompanionAilment::Weary:
        bSuited = (Source == ECareSource::Rest || Source == ECareSource::Play ||
                   Source == ECareSource::Meal || Source == ECareSource::Springs);
        break;
    }

    if (!bSuited)
    {
        return false;
    }
    ClearAilment(EcoKinID, Ailment);
    return true;
}

bool UCompanionCareSubsystem::SootheCompanion(int32 EcoKinID, ECareSource Source)
{
    return TreatAilment(EcoKinID, ECompanionAilment::Shaken, Source);
}

void UCompanionCareSubsystem::ExposeToBlight(int32 EcoKinID)
{
    FCompanionCondition* Cond = Conditions.Find(EcoKinID);
    if (!Cond || Cond->HasAilment(ECompanionAilment::Ailing))
    {
        return;
    }
    // Tired bodies catch what's in the air: Weary workers are likelier
    // to fall Ailing in blight.
    const float Risk = Cond->HasAilment(ECompanionAilment::Weary) ? 0.45f : 0.15f;
    if (FMath::FRand() < Risk)
    {
        GainAilment(EcoKinID, ECompanionAilment::Ailing);
    }
}

void UCompanionCareSubsystem::ApplyHeavyStrain(int32 EcoKinID)
{
    FCompanionCondition* Cond = Conditions.Find(EcoKinID);
    if (!Cond || Cond->HasAilment(ECompanionAilment::Shaken))
    {
        return;
    }
    if (FMath::FRand() < 0.5f)
    {
        GainAilment(EcoKinID, ECompanionAilment::Shaken);
    }
}

FCompanionCondition UCompanionCareSubsystem::GetCondition(int32 EcoKinID) const
{
    const FCompanionCondition* Cond = Conditions.Find(EcoKinID);
    return Cond ? *Cond : FCompanionCondition();
}

void UCompanionCareSubsystem::WorkTick()
{
    TArray<int32> NowWeary;
    for (int32 EcoKinID : Workers)
    {
        FCompanionCondition* Cond = Conditions.Find(EcoKinID);
        if (!Cond || Cond->HasAilment(ECompanionAilment::Ailing))
        {
            continue; // The sick don't work. Full stop.
        }
        Cond->Energy = FMath::Max(0.0f, Cond->Energy - WorkDrainPerSecond);
        OnEnergyChanged.Broadcast(EcoKinID, Cond->Energy);
        if (Cond->Energy <= 0.0f && !Cond->HasAilment(ECompanionAilment::Weary))
        {
            NowWeary.Add(EcoKinID);
        }
    }
    for (int32 EcoKinID : NowWeary)
    {
        GainAilment(EcoKinID, ECompanionAilment::Weary);
    }
}

void UCompanionCareSubsystem::GainAilment(int32 EcoKinID, ECompanionAilment Ailment)
{
    FCompanionCondition* Cond = Conditions.Find(EcoKinID);
    if (!Cond || Cond->HasAilment(Ailment))
    {
        return;
    }
    Cond->Ailments.Add(Ailment);
    OnAilmentGained.Broadcast(EcoKinID, Ailment);
}

void UCompanionCareSubsystem::ClearAilment(int32 EcoKinID, ECompanionAilment Ailment)
{
    FCompanionCondition* Cond = Conditions.Find(EcoKinID);
    if (!Cond || !Cond->HasAilment(Ailment))
    {
        return;
    }
    Cond->Ailments.Remove(Ailment);
    OnAilmentCleared.Broadcast(EcoKinID, Ailment);
}
