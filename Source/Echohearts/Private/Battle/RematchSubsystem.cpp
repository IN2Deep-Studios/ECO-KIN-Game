// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Battle/RematchSubsystem.h"

void URematchSubsystem::RegisterBoss(int32 BossDexID, int32 MaxTier)
{
    if (BossDexID <= 0 || MaxTier < 1)
    {
        return;
    }
    MaxTiers.Add(BossDexID, MaxTier);
    HighestCleared.Add(BossDexID, 0);
}

bool URematchSubsystem::StartRematch(int32 BossDexID, int32 Tier)
{
    const int32* Max = MaxTiers.Find(BossDexID);
    const int32* Cleared = HighestCleared.Find(BossDexID);
    if (!Max || !Cleared || Tier < 1 || Tier > *Max || Tier > *Cleared + 1)
    {
        return false; // Tiers unlock in order — earn the next one.
    }
    return true;
}

void URematchSubsystem::ReportRematchVictory(int32 BossDexID)
{
    int32* Cleared = HighestCleared.Find(BossDexID);
    const int32* Max = MaxTiers.Find(BossDexID);
    if (!Cleared || !Max)
    {
        return;
    }
    if (*Cleared < *Max)
    {
        ++(*Cleared);
        OnRematchTierUnlocked.Broadcast(BossDexID, *Cleared);
    }
}

int32 URematchSubsystem::GetHighestTierCleared(int32 BossDexID) const
{
    const int32* Cleared = HighestCleared.Find(BossDexID);
    return Cleared ? *Cleared : 0;
}
