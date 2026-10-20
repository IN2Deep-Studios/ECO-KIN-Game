// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Battle/StrongholdSubsystem.h"

void UStrongholdSubsystem::RegisterStronghold(FName StrongholdId, const FText& Title, int32 ObjectiveCount)
{
    if (StrongholdId.IsNone() || ObjectiveCount <= 0)
    {
        return;
    }
    ObjectiveTotals.Add(StrongholdId, ObjectiveCount);
    ObjectivesDone.Add(StrongholdId, 0);
    States.Add(StrongholdId, EAssaultState::Quiet);
}

bool UStrongholdSubsystem::StartAssault(FName StrongholdId, EAssaultApproach Approach)
{
    EAssaultState* State = States.Find(StrongholdId);
    if (!State || *State == EAssaultState::Underway || *State == EAssaultState::Breached)
    {
        return false;
    }
    *State = EAssaultState::Underway;
    ObjectivesDone.Add(StrongholdId, 0);
    OnAssaultStarted.Broadcast(StrongholdId, Approach);
    return true;
}

void UStrongholdSubsystem::ReportObjectiveComplete(FName StrongholdId)
{
    EAssaultState* State = States.Find(StrongholdId);
    int32* Done = ObjectivesDone.Find(StrongholdId);
    const int32* Total = ObjectiveTotals.Find(StrongholdId);
    if (!State || !Done || !Total || *State == EAssaultState::Quiet)
    {
        return;
    }
    ++(*Done);
    if (*Done >= *Total / 2 && *State == EAssaultState::Underway)
    {
        *State = EAssaultState::Breached; // inner ward down — the heart is exposed
    }
    if (*Done >= *Total)
    {
        *State = EAssaultState::Completed;
        OnAssaultEnded.Broadcast(StrongholdId, true);
    }
}

void UStrongholdSubsystem::ReportRepelled(FName StrongholdId)
{
    EAssaultState* State = States.Find(StrongholdId);
    int32* Total = ObjectiveTotals.Find(StrongholdId);
    if (!State)
    {
        return;
    }
    *State = EAssaultState::Repelled;
    if (Total)
    {
        *Total += 1; // They reinforce. Next attempt hits harder.
    }
    OnAssaultEnded.Broadcast(StrongholdId, false);
}

EAssaultState UStrongholdSubsystem::GetAssaultState(FName StrongholdId) const
{
    const EAssaultState* State = States.Find(StrongholdId);
    return State ? *State : EAssaultState::Quiet;
}
