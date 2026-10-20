// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Adventure/BlightHuntSubsystem.h"

bool UBlightHuntSubsystem::StartHunt(FName HuntId, FName RegionId)
{
    if (HuntId.IsNone() || RegionId.IsNone())
    {
        return false;
    }
    States.Add(HuntId, EBlightHuntState::Tracking);
    TrailsFound.Add(HuntId, 0);
    OnHuntStage.Broadcast(HuntId);
    return true;
}

void UBlightHuntSubsystem::ReportTrailFound(FName HuntId, int32 TrailsTotal)
{
    EBlightHuntState* State = States.Find(HuntId);
    int32* Found = TrailsFound.Find(HuntId);
    if (!State || !Found || *State != EBlightHuntState::Tracking)
    {
        return;
    }
    ++(*Found);
    if (*Found >= TrailsTotal)
    {
        *State = EBlightHuntState::AtSource;
        OnHuntStage.Broadcast(HuntId);
    }
}

void UBlightHuntSubsystem::ReportSourceReached(FName HuntId)
{
    EBlightHuntState* State = States.Find(HuntId);
    if (!State || *State != EBlightHuntState::AtSource)
    {
        return;
    }
    *State = EBlightHuntState::Cleansing;
    OnHuntStage.Broadcast(HuntId);
}

void UBlightHuntSubsystem::ReportRiteHeld(FName HuntId)
{
    EBlightHuntState* State = States.Find(HuntId);
    if (!State || *State != EBlightHuntState::Cleansing)
    {
        return;
    }
    *State = EBlightHuntState::Healed;
    OnHuntEnded.Broadcast(HuntId, true);
}

EBlightHuntState UBlightHuntSubsystem::GetHuntState(FName HuntId) const
{
    const EBlightHuntState* State = States.Find(HuntId);
    return State ? *State : EBlightHuntState::Idle;
}
