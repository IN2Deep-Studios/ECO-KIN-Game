// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Progression/BossProgressionTypes.h"
#include "BossProgressionSubsystem.generated.h"

// BOSS PROGRESSION — the world's trials, in order.
//
// Fourteen bosses, every one already in the game: the corrupted alphas of
// the Twelve Wounded, the Seven Beasts, champions of the secret societies,
// and a Shadow Beast Prime. Each is marked required or optional. A boss may
// only be challenged once every REQUIRED boss before it has fallen — the
// story gates itself, and the curious are rewarded for wandering.
//
// When threshold bosses fall, the world changes: skies clear, the land
// cools, the blight recedes, new regions open. Defeating a boss is never
// just a trophy — it is Rebearth breathing easier.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-16).
UCLASS()
class ECHOHEARTS_API UBossProgressionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Registers one boss def (data-side). The fourteen canonical bosses are
    // registered at world begin play, in order.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Bosses")
    void RegisterBossDef(const FBossDef& Def);

    // True when every REQUIRED boss before this one has fallen. Optional
    // bosses never block anything behind them.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Bosses")
    bool CanChallenge(FName BossId) const;

    // Marks a boss defeated. Fires its world transition if it has one, and
    // advances the world era when a threshold boss falls. Returns a readable
    // summary for UI ("Blightstorm has fallen. The skies clear.").
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Bosses")
    FString MarkBossDefeated(FName BossId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Bosses")
    bool IsBossDefeated(FName BossId) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Bosses")
    EWorldEra GetCurrentEra() const { return CurrentEra; }

    UFUNCTION(BlueprintPure, Category = "Echohearts|Bosses")
    int32 GetDefeatedCount() const { return DefeatedBosses.Num(); }

    UFUNCTION(BlueprintPure, Category = "Echohearts|Bosses")
    bool TryGetBossDef(FName BossId, FBossDef& OutDef) const;

    // Returns the phase list for a boss, in threshold order (100 first).
    // Phases live on the def; this is the read path the fight logic uses.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Bosses")
    bool TryGetBossPhases(FName BossId, TArray<FBossPhaseDef>& OutPhases) const;

protected:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
    void RegisterCanonicalBosses();
    void AddBoss(const TCHAR* Id, const TCHAR* Name, const TCHAR* Title, bool bRequired,
        int32 Order, int32 CorruptedDexID, const TCHAR* Hint,
        const TCHAR* TransitionId, const TCHAR* TransitionDesc);

    UPROPERTY()
    TMap<FName, FBossDef> BossDefs;

    UPROPERTY()
    TSet<FName> DefeatedBosses;

    UPROPERTY()
    TSet<FName> FiredTransitions;

    EWorldEra CurrentEra = EWorldEra::DawnOfRestoration;
};
