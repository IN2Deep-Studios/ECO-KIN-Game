// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Adventure/GlideRaceSubsystem.h"

int32 UGlideRaceSubsystem::StartRace(FName CourseId, int32 CheckpointCount)
{
    if (CourseId.IsNone() || CheckpointCount <= 0)
    {
        return 0;
    }
    const int32 RaceId = NextRaceId++;
    RaceCourses.Add(RaceId, CourseId);
    RaceCheckpoints.Add(RaceId, 0);
    return RaceId;
}

void UGlideRaceSubsystem::ReportCheckpoint(int32 RaceId)
{
    int32* Done = RaceCheckpoints.Find(RaceId);
    if (!Done)
    {
        return;
    }
    ++(*Done);
    OnRaceCheckpoint.Broadcast(RaceId, *Done);
}

void UGlideRaceSubsystem::FinishRace(int32 RaceId, float TimeSeconds)
{
    FName* Course = RaceCourses.Find(RaceId);
    if (!Course || TimeSeconds <= 0.0f)
    {
        return;
    }
    const float* Best = BestTimes.Find(*Course);
    const bool bNewRecord = !Best || TimeSeconds < *Best;
    if (bNewRecord)
    {
        BestTimes.Add(*Course, TimeSeconds);
    }
    RaceCourses.Remove(RaceId);
    RaceCheckpoints.Remove(RaceId);
    OnRaceFinished.Broadcast(RaceId, TimeSeconds, bNewRecord);
}

float UGlideRaceSubsystem::GetBestTime(FName CourseId) const
{
    const float* Best = BestTimes.Find(CourseId);
    return Best ? *Best : 0.0f;
}
