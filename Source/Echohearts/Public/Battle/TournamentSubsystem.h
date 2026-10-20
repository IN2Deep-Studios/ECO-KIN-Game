// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TournamentSubsystem.generated.h"

UENUM(BlueprintType)
enum class ETournamentState : uint8
{
    Quiet      UMETA(DisplayName = "Quiet"),
    Entered    UMETA(DisplayName = "Entered"),
    Semifinal  UMETA(DisplayName = "Semifinal"),
    Final      UMETA(DisplayName = "Final"),
    Champion   UMETA(DisplayName = "Champion"),
    Eliminated UMETA(DisplayName = "Eliminated")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTournamentStage, FName, TournamentId, ETournamentState, Stage);

// Circuit Championships: seasonal tournaments above the weekly
// circuit nights. Single elimination, best-of-three matches, the
// whole Sanctuary watching. Broadcast hooks already exist — this is
// the bracket.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UTournamentSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    void RegisterTournament(FName TournamentId, const FText& Title);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    bool EnterTournament(FName TournamentId);

    // Win your match: quarterfinal -> semifinal -> final -> champion.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    void ReportMatchWin(FName TournamentId);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Battle")
    void ReportMatchLoss(FName TournamentId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Battle")
    ETournamentState GetTournamentState(FName TournamentId) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Battle")
    FOnTournamentStage OnTournamentStage;

private:
    UPROPERTY()
    TMap<FName, ETournamentState> States;
};
