// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Discovery/JournalSubsystem.h"

void UJournalSubsystem::RegisterResearchTask(const FResearchTaskDef& Def)
{
    if (Def.TaskId.IsNone() || Def.TargetCount <= 0)
    {
        return;
    }
    ResearchTasks.Add(Def.TaskId, Def);
}

bool UJournalSubsystem::LogObservation(const FJournalObservation& Observation)
{
    if (Observation.EcoKinID <= 0)
    {
        return false;
    }
    // Breadth over grinding: the same species+behavior only counts once.
    for (const FJournalObservation& Existing : Observations)
    {
        if (Existing.EcoKinID == Observation.EcoKinID && Existing.Behavior == Observation.Behavior)
        {
            return false;
        }
    }

    Observations.Add(Observation);
    OnObservationLogged.Broadcast(Observation);

    for (auto& Pair : ResearchTasks)
    {
        const FName TaskId = Pair.Key;
        if (CompletedTasks.Contains(TaskId))
        {
            continue;
        }
        const FResearchTaskDef& Def = Pair.Value;
        if (TaskMatches(Def, Observation) && CountTaskProgress(Def) >= Def.TargetCount)
        {
            CompletedTasks.Add(TaskId);
            OnResearchTaskComplete.Broadcast(TaskId, Def.KindlingReward);
        }
    }
    return true;
}

int32 UJournalSubsystem::GetObservationCount() const
{
    return Observations.Num();
}

bool UJournalSubsystem::TaskMatches(const FResearchTaskDef& Def, const FJournalObservation& Obs) const
{
    if (!Def.SpeciesFilter.IsEmpty() && !Def.SpeciesFilter.Contains(Obs.EcoKinID))
    {
        return false;
    }
    if (!Def.BehaviorFilter.IsEmpty() && !Def.BehaviorFilter.Contains(Obs.Behavior))
    {
        return false;
    }
    return true;
}

int32 UJournalSubsystem::CountTaskProgress(const FResearchTaskDef& Def) const
{
    int32 Count = 0;
    for (const FJournalObservation& Obs : Observations)
    {
        if (TaskMatches(Def, Obs))
        {
            ++Count;
        }
    }
    return Count;
}
