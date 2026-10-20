// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Adventure/VoyageSubsystem.h"

bool UVoyageSubsystem::ChartVoyage(FName VoyageId, const FText& Destination, int32 WaypointCount)
{
    if (VoyageId.IsNone() || WaypointCount <= 0)
    {
        return false;
    }
    WaypointTotals.Add(VoyageId, WaypointCount);
    WaypointsReached.Add(VoyageId, 0);
    States.Add(VoyageId, EVoyageState::Charted);
    return true;
}

bool UVoyageSubsystem::BeginVoyage(FName VoyageId)
{
    EVoyageState* State = States.Find(VoyageId);
    if (!State || *State != EVoyageState::Charted)
    {
        return false;
    }
    *State = EVoyageState::Underway;
    return true;
}

void UVoyageSubsystem::ReportWaypointReached(FName VoyageId)
{
    EVoyageState* State = States.Find(VoyageId);
    int32* Reached = WaypointsReached.Find(VoyageId);
    const int32* Total = WaypointTotals.Find(VoyageId);
    if (!State || !Reached || !Total || *State != EVoyageState::Underway)
    {
        return;
    }
    ++(*Reached);
    OnVoyageWaypoint.Broadcast(VoyageId, *Reached);
    if (*Reached >= *Total)
    {
        *State = EVoyageState::Completed;
        OnVoyageEnded.Broadcast(VoyageId, true);
    }
}

EVoyageState UVoyageSubsystem::GetVoyageState(FName VoyageId) const
{
    const EVoyageState* State = States.Find(VoyageId);
    return State ? *State : EVoyageState::Charted;
}
