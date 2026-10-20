// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Adventure/TideSubsystem.h"

void UTideSubsystem::BeginTideWatch(float FullCycleSeconds)
{
    // The game clock drives ReportTidePhase; this just marks the watch
    // as active so the bell knows to ring.
}

void UTideSubsystem::ReportTidePhase(bool bLowTide)
{
    if (bLowTide == bIsLowTide)
    {
        return;
    }
    bIsLowTide = bLowTide;
    OnTideWindow.Broadcast(bIsLowTide);
}

void UTideSubsystem::ReportSecretFound(FName SecretId)
{
    if (SecretId.IsNone() || !bIsLowTide)
    {
        return; // The sea only loans its secrets at low tide.
    }
    FoundSecrets.Add(SecretId);
}
