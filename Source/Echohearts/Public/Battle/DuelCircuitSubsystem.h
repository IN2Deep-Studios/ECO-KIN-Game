// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Battle/DuelCircuitTypes.h"
#include "DuelCircuitSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCircuitRankUp, ECircuitRank, NewRank, ECircuitRank, OldRank);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCircuitBoutResult, FName, RivalId, bool, bWon);

// The Duel Circuit: a ranked sparring ladder at the Sanctuary proving
// grounds. Best-of-three duels, real-time, subdual or yield. Losing never
// costs Kindling — the Circuit is sport, and sport keeps blades sharp.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UDuelCircuitSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    void RegisterRival(const FRivalTamer& Rival);

    // Starts a bout with a rival (best of three). Returns false if the
    // rival is unknown or a bout is already running.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    bool StartBout(FName RivalId);

    // Reports one duel result within the active bout. First to 2 duel wins
    // takes the bout. A lost duel stings, never wounds: Kindling untouched.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    void ReportDuelResult(bool bWon);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Battle")
    ECircuitRank GetCircuitRank() const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Battle")
    FOnCircuitRankUp OnCircuitRankUp;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Battle")
    FOnCircuitBoutResult OnCircuitBoutResult;

private:
    void EndBout(bool bWon);
    void EvaluateRankUp();

    UPROPERTY()
    TMap<FName, FRivalTamer> Rivals;

    UPROPERTY()
    ECircuitRank CircuitRank = ECircuitRank::Hum;

    // Total bouts won (drives rank-ups: 3 per step).
    UPROPERTY()
    int32 TotalBoutVictories = 0;

    UPROPERTY()
    FName ActiveBoutRival = NAME_None;

    int32 ActiveBoutOurWins = 0;
    int32 ActiveBoutRivalWins = 0;
};
