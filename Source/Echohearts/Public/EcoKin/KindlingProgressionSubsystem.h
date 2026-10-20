// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EcoKin/EcoKinKindling.h"
#include "KindlingProgressionSubsystem.generated.h"

// What happened between a tamer and an Eco-Kin. Trust is earned through care,
// shared struggle, and restoration — never bought, never ground out.
UENUM(BlueprintType)
enum class EKindlingTrustEvent : uint8
{
    CareGiven            UMETA(DisplayName = "Care Given"),
    BattleFoughtTogether UMETA(DisplayName = "Battle Fought Together"),
    RestorationDeed      UMETA(DisplayName = "Restoration Deed"),
    TimeBonded           UMETA(DisplayName = "Time Bonded"),
    Neglect              UMETA(DisplayName = "Neglect"),
    HarmDone             UMETA(DisplayName = "Harm Done"),
    ForcedAction         UMETA(DisplayName = "Forced Action")
};

// Kindling TRUST PROGRESSION. Replaces the XP-grind feel entirely: there is no
// experience currency, no level, no grind loop in this subsystem. Trust rises
// through relationship events and falls through neglect or harm. A companion
// is a relationship, not a container.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API UKindlingProgressionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Records a trust event for a bonded Eco-Kin and returns the new trust %.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Kindling")
    float RecordTrustEvent(int32 EcoKinID, EKindlingTrustEvent Event);

    // Current Kindling state for a bonded Eco-Kin (false when never recorded).
    UFUNCTION(BlueprintPure, Category = "Echohearts|Kindling")
    bool TryGetKindling(int32 EcoKinID, FKindlingState& OutKindling) const;

    // Direct setter for load/save and tests. Trust stays clamped 0-100.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Kindling")
    void SetKindling(int32 EcoKinID, const FKindlingState& Kindling);

private:
    float TrustDeltaFor(EKindlingTrustEvent Event) const;

    UPROPERTY()
    TMap<int32, FKindlingState> KindlingByEcoKinID;
};
