// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Mission/HiddenMissionSubsystem.h"
#include "Encounter/WildEcoKinSpawner.h"
#include "Kismet/GameplayStatics.h"

void UHiddenMissionSubsystem::RegisterHiddenMission(const FHiddenMissionDef& Def)
{
    if (Def.MissionId.IsNone() || Def.Stages.IsEmpty() || Def.ExclusiveEcoKinID <= 0)
    {
        return;
    }
    Missions.Add(Def.MissionId, Def);
}

bool UHiddenMissionSubsystem::BeginMission(FName MissionId)
{
    if (!Missions.Contains(MissionId) || ActiveStages.Contains(MissionId) || CompletedMissions.Contains(MissionId))
    {
        return false;
    }
    ActiveStages.Add(MissionId, 0);
    return true;
}

bool UHiddenMissionSubsystem::ReportStageProgress(FName MissionId, int32 Count)
{
    int32* StageIdx = ActiveStages.Find(MissionId);
    FHiddenMissionDef* Def = Missions.Find(MissionId);
    if (!StageIdx || !Def || *StageIdx < 0 || *StageIdx >= Def->Stages.Num())
    {
        return false;
    }

    FHiddenMissionStage& Stage = Def->Stages[*StageIdx];
    Stage.CurrentCount = FMath::Min(Stage.TargetCount, Stage.CurrentCount + FMath::Max(0, Count));

    if (!Stage.IsDone())
    {
        return true;
    }

    const int32 NextIdx = *StageIdx + 1;
    if (NextIdx >= Def->Stages.Num())
    {
        CompleteMission(*Def);
        return true;
    }

    *StageIdx = NextIdx;
    OnHiddenMissionStageAdvanced.Broadcast(MissionId, NextIdx);
    return true;
}

bool UHiddenMissionSubsystem::IsMissionComplete(FName MissionId) const
{
    return CompletedMissions.Contains(MissionId);
}

int32 UHiddenMissionSubsystem::GetCurrentStageIndex(FName MissionId) const
{
    const int32* StageIdx = ActiveStages.Find(MissionId);
    return StageIdx ? *StageIdx : INDEX_NONE;
}

void UHiddenMissionSubsystem::CompleteMission(const FHiddenMissionDef& Def)
{
    ActiveStages.Remove(Def.MissionId);
    CompletedMissions.Add(Def.MissionId);

    // The exclusive species joins the wild population of its habitat
    // regions, and the map learns where to mark its presence.
    UnlockSpeciesInWild(Def);

    OnHiddenMissionCompleted.Broadcast(Def.MissionId, Def.ExclusiveEcoKinID, Def.HabitatRegionTags);
    OnExclusiveWildUnlock.Broadcast(Def.HabitatRegionTags);
}

void UHiddenMissionSubsystem::UnlockSpeciesInWild(const FHiddenMissionDef& Def)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    TArray<AActor*> Spawners;
    UGameplayStatics::GetAllActorsOfClass(World, AWildEcoKinSpawner::StaticClass(), Spawners);
    for (AActor* Actor : Spawners)
    {
        AWildEcoKinSpawner* Spawner = Cast<AWildEcoKinSpawner>(Actor);
        if (!Spawner)
        {
            continue;
        }
        // Spawners carry their region in Tags; only habitat regions unlock.
        for (const FName& RegionTag : Def.HabitatRegionTags)
        {
            if (Spawner->Tags.Contains(RegionTag))
            {
                Spawner->AddExclusiveSpecies(Def.ExclusiveEcoKinID, Def.WildUnlockWeight);
                break;
            }
        }
    }
}
