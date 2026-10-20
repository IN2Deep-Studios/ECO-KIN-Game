// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Battle/ArenaSubsystem.h"

int32 UArenaSubsystem::StartRun()
{
    const int32 RunId = NextRunId++;
    RunWaves.Add(RunId, 0);
    OnArenaWave.Broadcast(RunId, 0);
    return RunId;
}

void UArenaSubsystem::ReportWaveCleared(int32 RunId)
{
    int32* Waves = RunWaves.Find(RunId);
    if (!Waves)
    {
        return;
    }
    ++(*Waves);
    OnArenaWave.Broadcast(RunId, *Waves);
}

void UArenaSubsystem::EndRun(int32 RunId)
{
    int32* Waves = RunWaves.Find(RunId);
    if (!Waves)
    {
        return;
    }
    const int32 Cleared = *Waves;
    RunWaves.Remove(RunId);
    if (Cleared > BestWaves)
    {
        BestWaves = Cleared;
    }
    OnArenaRunEnded.Broadcast(RunId, Cleared);
}
