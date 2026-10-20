// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Progression/TriumphSubsystem.h"

void UTriumphSubsystem::RegisterTriumph(const FBossTriumph& Triumph)
{
    if (Triumph.BossDexID <= 0)
    {
        return;
    }
    Triumphs.Add(Triumph.BossDexID, Triumph);
}

void UTriumphSubsystem::ReportBossDefeated(int32 BossDexID)
{
    const FBossTriumph* Triumph = Triumphs.Find(BossDexID);
    if (!Triumph)
    {
        return;
    }
    TArray<FName> NewlyUnlocked;
    for (const FName& StructureId : Triumph->UnlockedStructureIds)
    {
        if (!UnlockedStructures.Contains(StructureId))
        {
            UnlockedStructures.Add(StructureId);
            NewlyUnlocked.Add(StructureId);
        }
    }
    OnTriumphProclaimed.Broadcast(BossDexID, NewlyUnlocked);
}

bool UTriumphSubsystem::IsStructureUnlocked(FName StructureId) const
{
    return UnlockedStructures.Contains(StructureId);
}

TArray<FName> UTriumphSubsystem::GetUnlockedStructures() const
{
    return UnlockedStructures.Array();
}
