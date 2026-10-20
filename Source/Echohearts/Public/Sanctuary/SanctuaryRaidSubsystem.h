// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sanctuary/SanctuaryRaidTypes.h"
#include "SanctuaryRaidSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRaidWarned, FName, RaidId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRaidWave, FName, RaidId, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRaidEnded, FName, RaidId, bool, bRepelled);

// Sanctuary Raids: periodic probes by the societies or the blight itself.
// Warning first (the Scout's sky-watch), waves second, aftermath third.
// The base defense rating softens the raid; residents fight beside the
// tamer. Raids damage, never destroy — repairs cost resources, flawless
// defenses earn the Warden track.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API USanctuaryRaidSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Sanctuary")
    void RegisterRaid(const FSanctuaryRaidDef& Def);

    // Schedules a raid: Warned now, waves after WarningSeconds.
    // DefenseRating comes from UBaseBuildingSubsystem::GetDefenseRating.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Sanctuary")
    bool TriggerRaid(FName RaidId, float DefenseRating);

    // Reports a wave fully resolved. When all waves resolve, the raid ends.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Sanctuary")
    void ReportWaveResolved(FName RaidId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    ERaidState GetRaidState(FName RaidId) const;

    // Escalation: the societies remember. Each breached raid raises this
    // raid's escalation level, multiplying its structure damage by
    // 1.0 + 0.25 per level. Spawners can read it to scale wave sizes too.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Sanctuary")
    float GetRaidDamageMultiplier(FName RaidId) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Sanctuary")
    FOnRaidWarned OnRaidWarned;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Sanctuary")
    FOnRaidWave OnRaidWave;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Sanctuary")
    FOnRaidEnded OnRaidEnded;

private:
    void BeginWaves(FName RaidId);
    void TickRaid(FName RaidId);
    void FinishRaid(FName RaidId, bool bRepelled);
    void ApplyRaidDamage(FName RaidId, int32 UnresolvedWaves);

    UPROPERTY()
    TMap<FName, FSanctuaryRaidDef> Raids;

    UPROPERTY()
    TMap<FName, ERaidState> States;

    UPROPERTY()
    TMap<FName, float> ElapsedSeconds;

    UPROPERTY()
    TMap<FName, int32> NextWaveIndex;

    UPROPERTY()
    TMap<FName, int32> ResolvedWaves;

    UPROPERTY()
    TMap<FName, float> CachedDefenseRating;

    // RaidId -> escalation level (breaches remembered).
    UPROPERTY()
    TMap<FName, int32> EscalationLevels;

    // RaidId -> seconds since last structure-damage tick.
    UPROPERTY()
    TMap<FName, float> DamageClocks;

    FTimerHandle RaidTimerHandle;
};
