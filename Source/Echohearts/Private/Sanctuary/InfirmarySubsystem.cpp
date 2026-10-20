// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Sanctuary/InfirmarySubsystem.h"
#include "BaseBuilding/BaseBuildingSubsystem.h"

void UInfirmarySubsystem::CollapseTamer(float StrainDebt)
{
    if (bTamerDown)
    {
        return;
    }
    bTamerDown = true;
    DownElapsedSeconds = 0.0f;
    DownRecoverySeconds = BaseRecoverySeconds + FMath::Max(0.0f, StrainDebt) * RecoverySecondsPerStrain;
    if (HasMendward())
    {
        DownRecoverySeconds *= 0.5f; // The hospital earns its keep.
    }
    OnTamerCollapsed.Broadcast(StrainDebt);

    if (!bTimerRunning && GetWorld())
    {
        FTimerDelegate TickDelegate = FTimerDelegate::CreateUObject(this, &UInfirmarySubsystem::RecoveryTick);
        GetWorld()->GetTimerManager().SetTimer(RecoveryTimerHandle, TickDelegate, 1.0f, true);
        bTimerRunning = true;
    }
}

void UInfirmarySubsystem::WakeTamer()
{
    if (!bTamerDown)
    {
        return;
    }
    bTamerDown = false;
    DownElapsedSeconds = 0.0f;
    // Waking up doesn't mean ready: the body keeps the score.
    RecoveringSeconds = 60.0f;
    OnTamerWoke.Broadcast(RecoveringSeconds);

    if (AdmittedResidents.IsEmpty() && bTimerRunning && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(RecoveryTimerHandle);
        bTimerRunning = false;
    }
}

void UInfirmarySubsystem::AdmitResident(FName ResidentId, float RecoveryHours)
{
    if (ResidentId.IsNone() || RecoveryHours <= 0.0f)
    {
        return;
    }
    AdmittedResidents.Add(ResidentId, RecoveryHours);
    OnResidentAdmitted.Broadcast(ResidentId);

    if (!bTimerRunning && GetWorld())
    {
        FTimerDelegate TickDelegate = FTimerDelegate::CreateUObject(this, &UInfirmarySubsystem::RecoveryTick);
        GetWorld()->GetTimerManager().SetTimer(RecoveryTimerHandle, TickDelegate, 1.0f, true);
        bTimerRunning = true;
    }
}

void UInfirmarySubsystem::DischargeResident(FName ResidentId)
{
    if (AdmittedResidents.Remove(ResidentId) > 0)
    {
        OnResidentDischarged.Broadcast(ResidentId);
    }
    if (!bTamerDown && AdmittedResidents.IsEmpty() && bTimerRunning && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(RecoveryTimerHandle);
        bTimerRunning = false;
    }
}

TArray<FName> UInfirmarySubsystem::GetAdmittedResidents() const
{
    TArray<FName> Ids;
    AdmittedResidents.GetKeys(Ids);
    return Ids;
}

float UInfirmarySubsystem::GetDefenseMultiplier() const
{
    return bTamerDown ? 0.5f : 1.0f;
}

float UInfirmarySubsystem::GetRaidVulnerability() const
{
    return bTamerDown ? 1.5f : 1.0f;
}

void UInfirmarySubsystem::RecoveryTick()
{
    if (bTamerDown)
    {
        DownElapsedSeconds += 1.0f;
        if (DownElapsedSeconds >= DownRecoverySeconds)
        {
            WakeTamer();
        }
    }

    if (RecoveringSeconds > 0.0f)
    {
        RecoveringSeconds = FMath::Max(0.0f, RecoveringSeconds - 1.0f);
    }

    // Resident recovery: game-hours tick here as seconds for simplicity;
    // the game layer maps this to its own clock.
    TArray<FName> Healed;
    for (auto& Pair : AdmittedResidents)
    {
        Pair.Value -= 1.0f / 3600.0f;
        if (Pair.Value <= 0.0f)
        {
            Healed.Add(Pair.Key);
        }
    }
    for (FName ResidentId : Healed)
    {
        DischargeResident(ResidentId);
    }

    if (!bTamerDown && AdmittedResidents.IsEmpty() && RecoveringSeconds <= 0.0f && bTimerRunning && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(RecoveryTimerHandle);
        bTimerRunning = false;
    }
}

bool UInfirmarySubsystem::HasMendward() const
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UBaseBuildingSubsystem* Base = GI->GetSubsystem<UBaseBuildingSubsystem>())
        {
            return Base->IsStructurePlaced(FName("mendward"));
        }
    }
    return false;
}
