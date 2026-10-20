// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "MissionTypes.generated.h"

// Mission kinds. Restoration is the central conflict of Rebearth; companion
// acquisition follows the 14-path relationship matrix (Binding through
// Resonance Alignment — never capture); story carries the world forward.
UENUM(BlueprintType)
enum class EMissionKind : uint8
{
    Restoration          UMETA(DisplayName = "Restoration"),
    CompanionAcquisition UMETA(DisplayName = "Companion Acquisition"),
    Story                UMETA(DisplayName = "Story"),
    Challenge            UMETA(DisplayName = "Challenge")
};

UENUM(BlueprintType)
enum class EMissionState : uint8
{
    Available UMETA(DisplayName = "Available"),
    Active    UMETA(DisplayName = "Active"),
    Complete  UMETA(DisplayName = "Complete"),
    Failed    UMETA(DisplayName = "Failed")
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FMissionObjective
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    int32 TargetCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    int32 CurrentCount = 0;

    bool IsDone() const { return CurrentCount >= TargetCount; }
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FMission
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    FName MissionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    EMissionKind Kind = EMissionKind::Restoration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    EMissionState State = EMissionState::Available;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    TArray<FMissionObjective> Objectives;

    // Kindling trust awarded on completion (restoration deeds pay the most —
    // the world heals when you heal it).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mission")
    float TrustReward = 5.0f;

    bool AllObjectivesDone() const
    {
        for (const FMissionObjective& O : Objectives)
        {
            if (!O.IsDone()) return false;
        }
        return !Objectives.IsEmpty();
    }
};
