// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Battle/BattleTypes.h"
#include "BattleSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnChainTriggered, FName, ChainId, AActor*, Instigator, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResonantClimaxChanged, bool, bActive);

// BATTLE MODES — Duel, Raid, Sparring.
//
// One orchestrator for every fight. It does not replace the strike pipeline
// (EssenceAbilitySubsystem still executes abilities); it wraps fights in
// modes, tracks Harmonic Momentum, resolves essence chains, and raises the
// Resonant Climax when both bonds are burning hot.
//
// Canon lines this file will not cross: combat is real-time; one deployed
// EcoKin per Tamer; every strike pays Anima-Link strain; no EXP anywhere
// (Sparring builds Kindling); nothing sentient is killed — raids end in
// Subdued or Purified, duels in yield or withdrawal.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-17).
UCLASS()
class ECHOHEARTS_API UBattleSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;

    // --- Mode lifecycle ---

    // 1v1: OpponentTamer may be a player character or an NPC duelist actor.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    bool StartDuel(AActor* ChallengerTamer, AActor* OpponentTamer);

    // Cooperative takedown of a giant corrupted EcoKin. Participants are
    // Tamer actors; allies' EcoKin fight beside them. BossId references
    // the boss progression roster; phases come from BossProgressionTypes.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    bool StartRaid(const TArray<AActor*>& ParticipantTamers, FName BossId, AActor* RaidTarget);

    // Consensual practice. Builds Kindling instead of dealing real harm.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    bool StartSparring(AActor* TamerA, AActor* TamerB);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    void EndBattle(EBattleResult Result);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Battle")
    bool IsBattleActive() const { return bBattleActive; }

    UFUNCTION(BlueprintPure, Category = "Echohearts|Battle")
    EBattleMode GetActiveMode() const { return ActiveMode; }

    // --- Momentum / chains / climax ---

    // Called by the ability pipeline after every strike. Returns true if a
    // chain triggered (ChainIdOut names it).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    bool NotifyAbilityUsed(AActor* Wielder, EEcoKinEssence Essence, AActor* Target, FName& ChainIdOut);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Battle")
    float GetMomentum(int32 EcoKinID) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Battle")
    bool IsResonantClimaxActive() const { return bClimaxActive; }

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Battle")
    FOnChainTriggered OnChainTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Battle")
    FOnResonantClimaxChanged OnResonantClimaxChanged;

protected:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
    void RegisterChainDefs();
    bool CheckChain(EEcoKinEssence Essence, FEssenceChain& Chain, FName& ChainIdOut) const;
    void UpdateMomentum(int32 EcoKinID, bool bOnPulse);
    void UpdateClimax();
    float GetLinkStrain(AActor* Tamer) const;
    int32 GetDeployedEcoKinID(AActor* Tamer) const;
    float GetWorldTime() const;

    bool bBattleActive = false;
    EBattleMode ActiveMode = EBattleMode::Duel;

    UPROPERTY()
    TMap<int32, FHarmonicMomentum> MomentumMap;

    UPROPERTY()
    TMap<int32, FEssenceChain> ChainMap;

    UPROPERTY()
    TArray<FEssenceChainDef> ChainDefs;

    // The two duelists' Tamers, for climax strain checks.
    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> DuelTamers;

    bool bClimaxActive = false;
    // Both bonds above this strain => Resonant Climax.
    float ClimaxStrainThreshold = 70.0f;
    float LastPulseTime = 0.0f;
};
