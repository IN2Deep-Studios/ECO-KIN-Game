// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Adventure/StormChaseSubsystem.h"

bool UStormChaseSubsystem::StartChase(FName StormId)
{
    if (StormId.IsNone() || ActiveStorms.Contains(StormId))
    {
        return false;
    }
    ActiveStorms.Add(StormId);
    return true;
}

void UStormChaseSubsystem::ReportRiding(FName StormId, float Intensity)
{
    if (!ActiveStorms.Contains(StormId))
    {
        return;
    }
    OnStormIntensity.Broadcast(StormId, FMath::Clamp(Intensity, 0.0f, 1.0f));
}

void UStormChaseSubsystem::EndChase(FName StormId, bool bRodeItOut)
{
    ActiveStorms.Remove(StormId);
}
