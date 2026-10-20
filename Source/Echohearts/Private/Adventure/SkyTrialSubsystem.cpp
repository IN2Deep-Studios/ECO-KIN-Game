// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Adventure/SkyTrialSubsystem.h"

bool USkyTrialSubsystem::StartTrial(ESkyIsle Isle)
{
    if (CompletedTrials.Contains(Isle) || ActiveTrials.Contains(Isle))
    {
        return false;
    }
    ActiveTrials.Add(Isle);
    return true;
}

void USkyTrialSubsystem::ReportTrialComplete(ESkyIsle Isle)
{
    if (!ActiveTrials.Contains(Isle))
    {
        return;
    }
    ActiveTrials.Remove(Isle);
    const bool bFirst = !CompletedTrials.Contains(Isle);
    CompletedTrials.Add(Isle);
    OnTrialCompleted.Broadcast(Isle, bFirst);
}

bool USkyTrialSubsystem::IsTrialComplete(ESkyIsle Isle) const
{
    return CompletedTrials.Contains(Isle);
}

int32 USkyTrialSubsystem::GetTrialsCompleted() const
{
    return CompletedTrials.Num();
}
