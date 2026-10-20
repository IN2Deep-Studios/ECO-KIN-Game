// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Adventure/EscortSubsystem.h"

bool UEscortSubsystem::StartEscort(FName EscortId, const FText& HerdName)
{
    if (EscortId.IsNone() || ActiveEscorts.Contains(EscortId))
    {
        return false;
    }
    ActiveEscorts.Add(EscortId);
    OnEscortProgress.Broadcast(EscortId, 0.0f);
    return true;
}

void UEscortSubsystem::ReportThreatRepelled(FName EscortId)
{
    if (!ActiveEscorts.Contains(EscortId))
    {
        return;
    }
    // Driven off, never killed — the escort's rule, enforced by the
    // encounter system, not by wishful thinking.
}

void UEscortSubsystem::ReportProgress(FName EscortId, float ProgressPercent)
{
    if (!ActiveEscorts.Contains(EscortId))
    {
        return;
    }
    const float Clamped = FMath::Clamp(ProgressPercent, 0.0f, 100.0f);
    OnEscortProgress.Broadcast(EscortId, Clamped);
    if (Clamped >= 100.0f)
    {
        ActiveEscorts.Remove(EscortId);
        OnEscortEnded.Broadcast(EscortId, true);
    }
}
