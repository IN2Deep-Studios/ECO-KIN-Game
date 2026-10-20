// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Sanctuary/WorkforceSubsystem.h"
#include "Companion/CompanionCareSubsystem.h"
#include "Companion/WorkAptitudeSubsystem.h"
#include "Sanctuary/InfirmarySubsystem.h"
#include "BaseBuilding/BaseBuildingSubsystem.h"

void UWorkforceSubsystem::RegisterJob(const FBaseJob& Job)
{
    if (Job.JobId.IsNone())
    {
        return;
    }
    Jobs.Add(Job.JobId, Job);
}

bool UWorkforceSubsystem::AssignCompanion(int32 EcoKinID, FName JobId, bool bAffinityMatch)
{
    FBaseJob* Job = Jobs.Find(JobId);
    if (!Job || EcoKinID <= 0 || Job->AssignedEcoKinID != 0)
    {
        return false;
    }
    Job->AssignedEcoKinID = EcoKinID;
    Job->bAffinityMatch = bAffinityMatch;

    // The right hands for the work: look up the companion's tier in the
    // job's required aptitude. Willing hands without the aptitude still
    // count — just for less.
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UWorkAptitudeSubsystem* Apt = GI->GetSubsystem<UWorkAptitudeSubsystem>())
        {
            Job->AptitudeTier = Apt->GetAptitudeTier(EcoKinID, Job->RequiredAptitude);
        }
    }

    // The work is real: the companion tires, and the care loop begins.
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UCompanionCareSubsystem* Care = GI->GetSubsystem<UCompanionCareSubsystem>())
        {
            Care->RegisterWorker(EcoKinID);
        }
    }
    OnJobAssigned.Broadcast(JobId, EcoKinID);
    return true;
}

bool UWorkforceSubsystem::VacateJob(FName JobId)
{
    FBaseJob* Job = Jobs.Find(JobId);
    if (!Job || Job->AssignedEcoKinID == 0)
    {
        return false;
    }
    const int32 WasAssigned = Job->AssignedEcoKinID;
    Job->AssignedEcoKinID = 0;
    Job->bAffinityMatch = false;
    Job->AptitudeTier = 0;

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UCompanionCareSubsystem* Care = GI->GetSubsystem<UCompanionCareSubsystem>())
        {
            Care->UnregisterWorker(WasAssigned);
        }
    }
    OnJobVacated.Broadcast(JobId);
    return true;
}

bool UWorkforceSubsystem::IsJobActive(FName JobId) const
{
    const FBaseJob* Job = Jobs.Find(JobId);
    if (!Job || Job->AssignedEcoKinID == 0)
    {
        return false;
    }
    // Both sides of the lean, verified: a fit companion AND a resident
    // on their feet.
    return IsCompanionFit(Job->AssignedEcoKinID) && IsResidentAvailable(Job->ResidentId);
}

TArray<FBaseJob> UWorkforceSubsystem::GetJobsForResident(FName ResidentId) const
{
    TArray<FBaseJob> Out;
    for (const auto& Pair : Jobs)
    {
        if (Pair.Value.ResidentId == ResidentId)
        {
            Out.Add(Pair.Value);
        }
    }
    return Out;
}

TArray<FBaseJob> UWorkforceSubsystem::GetVacantJobs() const
{
    TArray<FBaseJob> Out;
    for (const auto& Pair : Jobs)
    {
        if (Pair.Value.AssignedEcoKinID == 0)
        {
            Out.Add(Pair.Value);
        }
    }
    return Out;
}

float UWorkforceSubsystem::GetBaseHealth() const
{
    if (Jobs.IsEmpty())
    {
        return 0.0f;
    }

    int32 ActiveCount = 0;
    for (const auto& Pair : Jobs)
    {
        if (IsJobActive(Pair.Key))
        {
            ++ActiveCount;
        }
    }
    const float JobHealth = 100.0f * static_cast<float>(ActiveCount) / static_cast<float>(Jobs.Num());

    // Companion energy: the workforce's body.
    float EnergySum = 0.0f;
    int32 EnergyCount = 0;
    UGameInstance* GI = GetGameInstance();
    UCompanionCareSubsystem* Care = GI ? GI->GetSubsystem<UCompanionCareSubsystem>() : nullptr;
    if (Care)
    {
        for (const auto& Pair : Jobs)
        {
            if (Pair.Value.AssignedEcoKinID != 0)
            {
                EnergySum += Care->GetCondition(Pair.Value.AssignedEcoKinID).Energy;
                ++EnergyCount;
            }
        }
    }
    const float EnergyHealth = EnergyCount > 0 ? EnergySum / static_cast<float>(EnergyCount) : 100.0f;

    // Standing structures: the workforce's shelter.
    float StructureHealth = 100.0f;
    if (GI)
    {
        if (UBaseBuildingSubsystem* Base = GI->GetSubsystem<UBaseBuildingSubsystem>())
        {
            // Approximate: standing vs placed. (Placed count isn't
            // exposed; standing count alone scaled generously.)
            const int32 Standing = Base->GetStandingStructureIds().Num();
            StructureHealth = FMath::Min(100.0f, static_cast<float>(Standing) * 10.0f);
        }
    }

    return JobHealth * 0.5f + EnergyHealth * 0.3f + StructureHealth * 0.2f;
}

float UWorkforceSubsystem::GetBaseWealthPerHour() const
{
    float Wealth = 0.0f;
    for (const auto& Pair : Jobs)
    {
        if (IsJobActive(Pair.Key))
        {
            const float AptitudeEff = UWorkAptitudeSubsystem::EfficiencyForTier(Pair.Value.AptitudeTier);
            const float AffinityEff = Pair.Value.bAffinityMatch ? 1.5f : 1.0f;
            Wealth += Pair.Value.WealthPerHour * AptitudeEff * AffinityEff;
        }
    }
    return Wealth;
}

float UWorkforceSubsystem::GetWorkforceDefenseBonus() const
{
    float Bonus = 0.0f;
    for (const auto& Pair : Jobs)
    {
        if (Pair.Value.Category == EJobCategory::Defense && IsJobActive(Pair.Key))
        {
            const float AptitudeEff = UWorkAptitudeSubsystem::EfficiencyForTier(Pair.Value.AptitudeTier);
            const float AffinityEff = Pair.Value.bAffinityMatch ? 1.5f : 1.0f;
            Bonus += Pair.Value.DefenseBonus * AptitudeEff * AffinityEff;
        }
    }
    return Bonus;
}

bool UWorkforceSubsystem::IsCompanionFit(int32 EcoKinID) const
{
    UGameInstance* GI = GetGameInstance();
    UCompanionCareSubsystem* Care = GI ? GI->GetSubsystem<UCompanionCareSubsystem>() : nullptr;
    if (!Care)
    {
        return true;
    }
    return Care->GetCondition(EcoKinID).IsFitToWork();
}

bool UWorkforceSubsystem::IsResidentAvailable(FName ResidentId) const
{
    if (ResidentId.IsNone())
    {
        return true;
    }
    UGameInstance* GI = GetGameInstance();
    UInfirmarySubsystem* Infirmary = GI ? GI->GetSubsystem<UInfirmarySubsystem>() : nullptr;
    if (!Infirmary)
    {
        return true;
    }
    return !Infirmary->GetAdmittedResidents().Contains(ResidentId);
}
