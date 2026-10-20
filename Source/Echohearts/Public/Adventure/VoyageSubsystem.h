// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VoyageSubsystem.generated.h"

UENUM(BlueprintType)
enum class EVoyageState : uint8
{
    Charted   UMETA(DisplayName = "Charted"),
    Underway  UMETA(DisplayName = "Underway"),
    Completed UMETA(DisplayName = "Completed")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVoyageWaypoint, FName, VoyageId, int32, WaypointIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVoyageEnded, FName, VoyageId, bool, bCompleted);

// Elderwing Voyages: charted expeditions to uncharted regions. The
// ship is base-camp, her terraces are supply, and the sky is the road.
// Waypoints mark the journey; the destination marks the Tamer.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UVoyageSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    bool ChartVoyage(FName VoyageId, const FText& Destination, int32 WaypointCount);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    bool BeginVoyage(FName VoyageId);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportWaypointReached(FName VoyageId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Adventure")
    EVoyageState GetVoyageState(FName VoyageId) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnVoyageWaypoint OnVoyageWaypoint;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnVoyageEnded OnVoyageEnded;

private:
    UPROPERTY()
    TMap<FName, int32> WaypointTotals;

    UPROPERTY()
    TMap<FName, int32> WaypointsReached;

    UPROPERTY()
    TMap<FName, EVoyageState> States;
};
