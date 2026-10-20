// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "World/WorldSubsystem.h"

void UWorldSubsystem::RegisterRegion(const FRegionInfo& Region)
{
    Regions.Add(Region.RegionId, Region);
}

bool UWorldSubsystem::TryGetRegion(FName RegionId, FRegionInfo& OutRegion) const
{
    const FRegionInfo* Found = Regions.Find(RegionId);
    if (!Found)
    {
        return false;
    }
    OutRegion = *Found;
    return true;
}

void UWorldSubsystem::AddRestoration(FName RegionId, float Amount)
{
    FRegionInfo* Found = Regions.Find(RegionId);
    if (!Found || Amount <= 0.0f)
    {
        return;
    }
    const bool bWasRestored = Found->RestorationPercent >= 100.0f;
    Found->RestorationPercent = FMath::Min(100.0f, Found->RestorationPercent + Amount);
    if (!bWasRestored && Found->RestorationPercent >= 100.0f)
    {
        OnRegionRestored.Broadcast(RegionId);
    }
}

bool UWorldSubsystem::BeginTravel(FName DestinationRegionId, bool bByFlight)
{
    // Canon: travel is flight or ride, never teleportation. The travel itself
    // is a streamed journey; arrival fires OnTravelComplete.
    if (bTraveling || !Regions.Contains(DestinationRegionId) || DestinationRegionId == CurrentRegionId)
    {
        return false;
    }
    bTraveling = true;

    // Journey simulation placeholder: real implementation streams the world
    // along a flight path or mount run. Arrival completes the travel.
    CurrentRegionId = DestinationRegionId;
    bTraveling = false;
    OnTravelComplete.Broadcast(DestinationRegionId, bByFlight);
    return true;
}
