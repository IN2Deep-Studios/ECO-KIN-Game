// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Mission/HiddenMissionTypes.h"
#include "HiddenMissionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHiddenMissionStageAdvanced, FName, MissionId, int32, NewStageIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHiddenMissionCompleted, FName, MissionId, int32, ExclusiveEcoKinID, const TArray<FName>&, HabitatRegionTags);
// Fired so the world map can drop presence markers in the habitat regions.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExclusiveWildUnlock, const TArray<FName>&, HabitatRegionTags);

// Owns hidden-mission exclusives: missions that can only be found (never
// given), whose final stage offers Binding through Resonance Alignment, and
// whose completion releases the exclusive species into the wild population of
// its habitat regions. Exclusivity is a beginning, not a cage.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UHiddenMissionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Mission")
    void RegisterHiddenMission(const FHiddenMissionDef& Def);

    // Begins a discovered hidden mission. Returns false if unknown or already
    // begun/completed.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Mission")
    bool BeginMission(FName MissionId);

    // Reports progress on the current stage. Advances the stage when its
    // objective is met; completes the mission when the final stage is done.
    // A declined Binding offer simply leaves the final stage open — no
    // penalty, the mission waits.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Mission")
    bool ReportStageProgress(FName MissionId, int32 Count = 1);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Mission")
    bool IsMissionComplete(FName MissionId) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Mission")
    int32 GetCurrentStageIndex(FName MissionId) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Mission")
    FOnHiddenMissionStageAdvanced OnHiddenMissionStageAdvanced;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Mission")
    FOnHiddenMissionCompleted OnHiddenMissionCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Mission")
    FOnExclusiveWildUnlock OnExclusiveWildUnlock;

private:
    void CompleteMission(const FHiddenMissionDef& Def);
    void UnlockSpeciesInWild(const FHiddenMissionDef& Def);

    UPROPERTY()
    TMap<FName, FHiddenMissionDef> Missions;

    // MissionId -> current stage index. Absent = not begun.
    UPROPERTY()
    TMap<FName, int32> ActiveStages;

    UPROPERTY()
    TSet<FName> CompletedMissions;
};
