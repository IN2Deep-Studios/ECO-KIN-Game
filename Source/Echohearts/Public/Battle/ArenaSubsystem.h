// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ArenaSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnArenaWave, int32, RunId, int32, Wave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnArenaRunEnded, int32, RunId, int32, WavesCleared);

// The Arena of Echoes: wave-survival proving grounds. How many waves
// can you hold? Best runs are remembered per circuit rank — the
// leaderboard of the relentless.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UArenaSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    int32 StartRun();

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    void ReportWaveCleared(int32 RunId);

    // The run ends (yield or fall — falling here costs pride, not
    // partners; the arena is sport).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    void EndRun(int32 RunId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Battle")
    int32 GetBestWaves() const { return BestWaves; }

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Battle")
    FOnArenaWave OnArenaWave;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Battle")
    FOnArenaRunEnded OnArenaRunEnded;

private:
    UPROPERTY()
    TMap<int32, int32> RunWaves;

    UPROPERTY()
    int32 BestWaves = 0;

    int32 NextRunId = 1;
};
