// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Sanctuary/VesselSubsystem.h"

void UVesselSubsystem::NameVessel(const FText& NewName)
{
    if (!NewName.IsEmpty())
    {
        Vessel.VesselName = NewName;
    }
}

void UVesselSubsystem::LaunchVessel()
{
    Vessel.State = EVesselState::Aloft;
}

void UVesselSubsystem::DockVessel(EVesselState Dock)
{
    if (Dock == EVesselState::Aloft)
    {
        return;
    }
    Vessel.State = Dock;
}

bool UVesselSubsystem::AssignToRoost(int32 EcoKinID)
{
    if (EcoKinID <= 0 || Roost.Contains(EcoKinID) || ActiveTeam.Contains(EcoKinID))
    {
        return false;
    }
    if (Roost.Num() >= Vessel.RoostCapacity)
    {
        return false;
    }
    Roost.Add(EcoKinID);
    OnRosterChanged.Broadcast();
    return true;
}

bool UVesselSubsystem::AssignToActiveTeam(int32 EcoKinID)
{
    if (EcoKinID <= 0 || ActiveTeam.Contains(EcoKinID))
    {
        return false;
    }
    if (ActiveTeam.Num() >= ActiveTeamMaxSize)
    {
        return false;
    }
    Roost.Remove(EcoKinID);
    ActiveTeam.Add(EcoKinID);
    OnRosterChanged.Broadcast();
    return true;
}

bool UVesselSubsystem::RemoveFromVessel(int32 EcoKinID)
{
    const bool bRemoved = Roost.Remove(EcoKinID) > 0 || ActiveTeam.Remove(EcoKinID) > 0;
    if (bRemoved)
    {
        OnRosterChanged.Broadcast();
    }
    return bRemoved;
}

TArray<int32> UVesselSubsystem::GetRoost() const
{
    return Roost;
}

TArray<int32> UVesselSubsystem::GetActiveTeam() const
{
    return ActiveTeam;
}

bool UVesselSubsystem::CleanseTerrace(float RegionRestorationPercent)
{
    if (Vessel.GardenTerracesCleansed >= Vessel.GardenTerracesTotal)
    {
        return false;
    }
    // A terrace opens per restoration threshold crossed: the gardens
    // heal as Rebearth heals. No buying progress with coins.
    const int32 Earned = FMath::FloorToInt(RegionRestorationPercent / RestorationPerTerrace);
    if (Earned <= Vessel.GardenTerracesCleansed)
    {
        return false;
    }
    ++Vessel.GardenTerracesCleansed;
    OnTerraceCleansed.Broadcast(Vessel.GardenTerracesCleansed);
    return true;
}

bool UVesselSubsystem::UpgradeVessel(EUpgradePath Path)
{
    ++Vessel.Tier;
    if (Path == EUpgradePath::Deepen)
    {
        // Richer roost: deeper recovery, deeper gardens.
    }
    else
    {
        // Widen: more berths, more terraces.
        Vessel.RoostCapacity += 6;
        Vessel.GardenTerracesTotal += 2;
    }
    // The inventory system charges shed/gifted costs on this broadcast.
    OnVesselUpgraded.Broadcast(Vessel.Tier);
    return true;
}

float UVesselSubsystem::GetRoostRecoveryBonus() const
{
    // Roosting companions rest deeply: 1.5x recovery, +0.25 per tier
    // beyond the first.
    return 1.5f + 0.25f * static_cast<float>(Vessel.Tier - 1);
}
