// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InfirmarySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTamerCollapsed, float, StrainDebt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTamerWoke, float, RecoveringSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResidentAdmitted, FName, ResidentId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResidentDischarged, FName, ResidentId);

// The Infirmary: when the Tamer's stamina hits zero — the Anima-Link's
// bidirectional price — the Tamer passes out and wakes in the hospital
// (the Mendward, if built). While the Tamer is down, the base is open
// and vulnerable: defense is halved, raids press harder, and the
// residents hold the line alone. Residents hurt in raids are admitted
// too, and their services wait until they walk out.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UInfirmarySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // The Tamer collapses. StrainDebt scales the recovery time.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Sanctuary")
    void CollapseTamer(float StrainDebt);

    // Wakes the Tamer early (Mender intervention). Still Recovering.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Sanctuary")
    void WakeTamer();

    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    bool IsTamerDown() const { return bTamerDown; }

    // Admit a resident hurt in a raid. Their services are unavailable
    // until discharged.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Sanctuary")
    void AdmitResident(FName ResidentId, float RecoveryHours);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Sanctuary")
    void DischargeResident(FName ResidentId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    TArray<FName> GetAdmittedResidents() const;

    // Defense multiplier while the Tamer is down: 0.5 — the base is open
    // and vulnerable with no commander. 1.0 otherwise.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    float GetDefenseMultiplier() const;

    // Raid scheduler pressure: 1.5 while the Tamer is down (raids come
    // sooner and hit with less warning). 1.0 otherwise.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    float GetRaidVulnerability() const;

    // Seconds of the Recovering debuff left (stamina drains faster).
    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    float GetRecoveringTimeRemaining() const { return RecoveringSeconds; }

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Sanctuary")
    FOnTamerCollapsed OnTamerCollapsed;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Sanctuary")
    FOnTamerWoke OnTamerWoke;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Sanctuary")
    FOnResidentAdmitted OnResidentAdmitted;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Sanctuary")
    FOnResidentDischarged OnResidentDischarged;

    // Base recovery seconds per point of strain debt (halved by Mendward).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    float RecoverySecondsPerStrain = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    float BaseRecoverySeconds = 20.0f;

private:
    void RecoveryTick();
    bool HasMendward() const;

    UPROPERTY()
    bool bTamerDown = false;

    UPROPERTY()
    float DownElapsedSeconds = 0.0f;

    UPROPERTY()
    float DownRecoverySeconds = 20.0f;

    UPROPERTY()
    float RecoveringSeconds = 0.0f;

    UPROPERTY()
    TMap<FName, float> AdmittedResidents; // ResidentId -> hours remaining

    FTimerHandle RecoveryTimerHandle;
    bool bTimerRunning = false;
};
