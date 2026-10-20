// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Mission/MissionTypes.h"
#include "MissionSubsystem.generated.h"

// Owns the mission journal: available / active / complete missions, objective
// progress, and completion rewards. Restoration deeds completed here are what
// feed ecology accomplishments into Kindling and Growth Rite gates.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API UMissionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Registers a mission definition (usually from data at startup).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Mission")
    void RegisterMission(const FMission& Mission);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Mission")
    bool AcceptMission(FName MissionId);

    // Reports progress on an objective by index. Completes the mission when
    // all objectives are done; returns the trust reward earned (0 if not).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Mission")
    float ReportObjectiveProgress(FName MissionId, int32 ObjectiveIndex, int32 Count = 1);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Mission")
    bool TryGetMission(FName MissionId, FMission& OutMission) const;

    // How many restoration deeds the tamer has completed (feeds Growth Rites).
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Mission")
    int32 CompletedRestorationDeeds = 0;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMissionComplete, FName, MissionId, float, TrustReward);

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Mission")
    FOnMissionComplete OnMissionComplete;

private:
    UPROPERTY()
    TMap<FName, FMission> Missions;
};
