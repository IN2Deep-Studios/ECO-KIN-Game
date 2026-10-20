// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Discovery/JournalTypes.h"
#include "JournalSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObservationLogged, const FJournalObservation&, Observation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResearchTaskComplete, FName, TaskId, float, KindlingReward);

// The Field Journal: A.E.G.I.S. field research as progression. Log
// observations of wild EcoKin behavior; complete research tasks for Kindling
// trust and ecology accomplishments. Studying pays — the anti-Pokédex.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UJournalSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    void RegisterResearchTask(const FResearchTaskDef& Def);

    // Logs one observation. Returns false for invalid/duplicate entries.
    // Duplicates (same species + behavior) don't double-count toward tasks.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    bool LogObservation(const FJournalObservation& Observation);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Discovery")
    int32 GetObservationCount() const;

    UFUNCTION(BlueprintAssignable, Category = "Echohearts|Discovery")
    FOnObservationLogged OnObservationLogged;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Discovery")
    FOnResearchTaskComplete OnResearchTaskComplete;

private:
    bool TaskMatches(const FResearchTaskDef& Def, const FJournalObservation& Obs) const;
    int32 CountTaskProgress(const FResearchTaskDef& Def) const;

    UPROPERTY()
    TArray<FJournalObservation> Observations;

    UPROPERTY()
    TMap<FName, FResearchTaskDef> ResearchTasks;

    UPROPERTY()
    TSet<FName> CompletedTasks;
};
