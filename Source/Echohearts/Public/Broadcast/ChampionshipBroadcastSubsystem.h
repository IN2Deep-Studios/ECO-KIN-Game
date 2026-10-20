// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ChampionshipBroadcastSubsystem.generated.h"

class AChampionshipTV;

// UChampionshipBroadcastSubsystem: world-level schedule for championship
// broadcast TVs. Registers every AChampionshipTV in the level and can push a
// single "live reel" to all of them at once (arena-wide broadcast control).
//
// CANON LOCK: championship matches are exhibition sport between willing EcoKin.
// This subsystem only switches recordings; it never touches capture, bonding,
// or any EcoKin directly.
//
// EVIDENCE TIER: IMPLEMENTED only. NOT COMPILED, NOT TESTED, NOT RUNTIME VERIFIED.
UCLASS()
class ECHOHEARTS_API UChampionshipBroadcastSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Registers/unregisters a TV for arena-wide broadcast control.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Broadcast")
    void RegisterTV(AChampionshipTV* TV);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Broadcast")
    void UnregisterTV(AChampionshipTV* TV);

    // Pushes one reel to every registered TV at once (the "live" broadcast).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Broadcast")
    void SetLiveReel(int32 ReelIndex);

    // Stops all registered TVs.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Broadcast")
    void StopAllBroadcasts();

    UFUNCTION(BlueprintPure, Category = "Echohearts|Broadcast")
    int32 GetRegisteredTVCount() const;

private:
    // Weak refs: TVs may be streamed out or destroyed without notice.
    UPROPERTY(Transient)
    TArray<TWeakObjectPtr<AChampionshipTV>> RegisteredTVs;
};
