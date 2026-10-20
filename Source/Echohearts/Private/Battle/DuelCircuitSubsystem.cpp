// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Battle/DuelCircuitSubsystem.h"

void UDuelCircuitSubsystem::RegisterRival(const FRivalTamer& Rival)
{
    if (Rival.RivalId.IsNone())
    {
        return;
    }
    Rivals.Add(Rival.RivalId, Rival);
}

bool UDuelCircuitSubsystem::StartBout(FName RivalId)
{
    if (!Rivals.Contains(RivalId) || ActiveBoutRival != NAME_None)
    {
        return false;
    }
    ActiveBoutRival = RivalId;
    ActiveBoutOurWins = 0;
    ActiveBoutRivalWins = 0;
    return true;
}

void UDuelCircuitSubsystem::ReportDuelResult(bool bWon)
{
    if (ActiveBoutRival == NAME_None)
    {
        return;
    }

    if (bWon)
    {
        ++ActiveBoutOurWins;
    }
    else
    {
        ++ActiveBoutRivalWins;
    }

    // First to 2 duel wins takes the bout.
    if (ActiveBoutOurWins >= 2)
    {
        EndBout(true);
    }
    else if (ActiveBoutRivalWins >= 2)
    {
        EndBout(false);
    }
}

ECircuitRank UDuelCircuitSubsystem::GetCircuitRank() const
{
    return CircuitRank;
}

void UDuelCircuitSubsystem::EndBout(bool bWon)
{
    const FName RivalId = ActiveBoutRival;
    ActiveBoutRival = NAME_None;
    ActiveBoutOurWins = 0;
    ActiveBoutRivalWins = 0;

    if (bWon)
    {
        ++TotalBoutVictories;
        EvaluateRankUp();
    }
    OnCircuitBoutResult.Broadcast(RivalId, bWon);
}

void UDuelCircuitSubsystem::EvaluateRankUp()
{
    // 3 bout victories per rank step: Hum -> Chord -> Chorus -> Symphony.
    ECircuitRank NewRank = CircuitRank;
    if (TotalBoutVictories >= 9)      NewRank = ECircuitRank::Symphony;
    else if (TotalBoutVictories >= 6) NewRank = ECircuitRank::Chorus;
    else if (TotalBoutVictories >= 3) NewRank = ECircuitRank::Chord;

    if (NewRank != CircuitRank)
    {
        const ECircuitRank Old = CircuitRank;
        CircuitRank = NewRank;
        OnCircuitRankUp.Broadcast(NewRank, Old);
    }
}
