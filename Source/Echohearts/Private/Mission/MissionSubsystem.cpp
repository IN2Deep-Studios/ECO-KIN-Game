// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Mission/MissionSubsystem.h"

void UMissionSubsystem::RegisterMission(const FMission& Mission)
{
    Missions.Add(Mission.MissionId, Mission);
}

bool UMissionSubsystem::AcceptMission(FName MissionId)
{
    FMission* Found = Missions.Find(MissionId);
    if (!Found || Found->State != EMissionState::Available)
    {
        return false;
    }
    Found->State = EMissionState::Active;
    return true;
}

float UMissionSubsystem::ReportObjectiveProgress(FName MissionId, int32 ObjectiveIndex, int32 Count)
{
    FMission* Found = Missions.Find(MissionId);
    if (!Found || Found->State != EMissionState::Active)
    {
        return 0.0f;
    }
    if (!Found->Objectives.IsValidIndex(ObjectiveIndex) || Count <= 0)
    {
        return 0.0f;
    }

    FMissionObjective& Objective = Found->Objectives[ObjectiveIndex];
    Objective.CurrentCount = FMath::Min(Objective.TargetCount, Objective.CurrentCount + Count);

    if (Found->AllObjectivesDone())
    {
        Found->State = EMissionState::Complete;
        if (Found->Kind == EMissionKind::Restoration)
        {
            CompletedRestorationDeeds++;
        }
        OnMissionComplete.Broadcast(MissionId, Found->TrustReward);
        return Found->TrustReward;
    }
    return 0.0f;
}

bool UMissionSubsystem::TryGetMission(FName MissionId, FMission& OutMission) const
{
    const FMission* Found = Missions.Find(MissionId);
    if (!Found)
    {
        return false;
    }
    OutMission = *Found;
    return true;
}
