// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sanctuary/WorkforceTypes.h"
#include "WorkforceSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJobAssigned, FName, JobId, int32, EcoKinID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJobVacated, FName, JobId);

// The Workforce: every resident leans on a companion, and every
// companion leans on the residents. Jobs are the unit of that trust:
// one resident's craft, one companion's hands. A job is ACTIVE only
// when a fit companion holds it and its resident is on their feet.
// Filled jobs make the base healthy and wealthy; empty ones are felt.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UWorkforceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Workforce")
    void RegisterJob(const FBaseJob& Job);

    // Assigns a companion to a job. bAffinityMatch when the companion's
    // essence suits the work (50% better output). Registers them as a
    // worker with the care subsystem — the work is real, and tiring.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Workforce")
    bool AssignCompanion(int32 EcoKinID, FName JobId, bool bAffinityMatch);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Workforce")
    bool VacateJob(FName JobId);

    // True when a fit companion holds the job and its resident is out
    // of the infirmary. The lean, verified both ways.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Workforce")
    bool IsJobActive(FName JobId) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Workforce")
    TArray<FBaseJob> GetJobsForResident(FName ResidentId) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Workforce")
    TArray<FBaseJob> GetVacantJobs() const;

    // 0-100. Filled active jobs, companion energy, standing structures.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Workforce")
    float GetBaseHealth() const;

    // Wealth per hour from active jobs (affinity-boosted where earned).
    UFUNCTION(BlueprintPure, Category = "Echohearts|Workforce")
    float GetBaseWealthPerHour() const;

    // Defense bonus from active Guard jobs — added to the structure
    // defense rating by the game layer.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Workforce")
    float GetWorkforceDefenseBonus() const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Workforce")
    FOnJobAssigned OnJobAssigned;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Workforce")
    FOnJobVacated OnJobVacated;

private:
    bool IsCompanionFit(int32 EcoKinID) const;
    bool IsResidentAvailable(FName ResidentId) const;

    UPROPERTY()
    TMap<FName, FBaseJob> Jobs;
};
