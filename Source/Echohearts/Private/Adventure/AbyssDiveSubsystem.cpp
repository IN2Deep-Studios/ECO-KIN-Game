// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Adventure/AbyssDiveSubsystem.h"

int32 UAbyssDiveSubsystem::StartDive()
{
    const int32 DiveId = NextDiveId++;
    ActiveDives.Add(DiveId);
    return DiveId;
}

void UAbyssDiveSubsystem::ReportDepth(int32 DiveId, float DepthMeters)
{
    if (!ActiveDives.Contains(DiveId) || DepthMeters < 0.0f)
    {
        return;
    }
    OnDiveDepth.Broadcast(DiveId, DepthMeters);
}

void UAbyssDiveSubsystem::ReportDiscovery(int32 DiveId, FName DiscoveryId)
{
    if (!ActiveDives.Contains(DiveId) || DiscoveryId.IsNone())
    {
        return;
    }
    OnAbyssDiscovery.Broadcast(DiveId, DiscoveryId);
}

void UAbyssDiveSubsystem::EndDive(int32 DiveId, bool bSurfaced)
{
    ActiveDives.Remove(DiveId);
}
