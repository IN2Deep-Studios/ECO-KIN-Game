// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SummoningTypes.generated.h"

// Why a summon call was refused. Refusals are always explained — a tamer
// should never wonder why their partner didn't answer.
UENUM(BlueprintType)
enum class ESummonRefusalReason : uint8
{
    None             UMETA(DisplayName = "None"),
    NotBonded        UMETA(DisplayName = "Not Bonded"),
    AlreadyPresent   UMETA(DisplayName = "Already Present"),
    Unanswered       UMETA(DisplayName = "Unanswered"),       // trust too low; the call goes unanswered
    Recovering       UMETA(DisplayName = "Recovering"),       // partner is withdrawn and healing
    InsufficientCharge UMETA(DisplayName = "Insufficient Charge"), // Eco-Device charge too low
    LinkCannotBear   UMETA(DisplayName = "Link Cannot Bear"), // strain would fracture the bond
    CeremonyLocked   UMETA(DisplayName = "Ceremony Locked")   // rites are never interrupted
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FSummonResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Summoning")
    bool bSucceeded = false;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Summoning")
    ESummonRefusalReason RefusalReason = ESummonRefusalReason::None;

    // Strain actually paid through the Anima-Link for this summon.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Summoning")
    float StrainPaid = 0.0f;

    static FSummonResult Ok(float InStrainPaid)
    {
        FSummonResult R;
        R.bSucceeded = true;
        R.StrainPaid = InStrainPaid;
        return R;
    }

    static FSummonResult Refused(ESummonRefusalReason Reason)
    {
        FSummonResult R;
        R.bSucceeded = false;
        R.RefusalReason = Reason;
        return R;
    }
};

// One bonded partner on the tamer's roster.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FBondedPartner
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Summoning")
    int32 EcoKinID = 0;

    // Whether this partner is currently deployed beside the tamer.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Summoning")
    bool bDeployed = false;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Summoning")
    TWeakObjectPtr<AActor> CompanionActor;
};
