// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Battle/TournamentSubsystem.h"

void UTournamentSubsystem::RegisterTournament(FName TournamentId, const FText& Title)
{
    if (TournamentId.IsNone())
    {
        return;
    }
    States.Add(TournamentId, ETournamentState::Quiet);
}

bool UTournamentSubsystem::EnterTournament(FName TournamentId)
{
    ETournamentState* State = States.Find(TournamentId);
    if (!State || *State != ETournamentState::Quiet)
    {
        return false;
    }
    *State = ETournamentState::Entered;
    OnTournamentStage.Broadcast(TournamentId, *State);
    return true;
}

void UTournamentSubsystem::ReportMatchWin(FName TournamentId)
{
    ETournamentState* State = States.Find(TournamentId);
    if (!State)
    {
        return;
    }
    switch (*State)
    {
    case ETournamentState::Entered:   *State = ETournamentState::Semifinal; break;
    case ETournamentState::Semifinal: *State = ETournamentState::Final;     break;
    case ETournamentState::Final:     *State = ETournamentState::Champion;  break;
    default: return;
    }
    OnTournamentStage.Broadcast(TournamentId, *State);
}

void UTournamentSubsystem::ReportMatchLoss(FName TournamentId)
{
    ETournamentState* State = States.Find(TournamentId);
    if (!State)
    {
        return;
    }
    if (*State == ETournamentState::Champion || *State == ETournamentState::Eliminated ||
        *State == ETournamentState::Quiet)
    {
        return;
    }
    *State = ETournamentState::Eliminated; // Single elimination. Ouch. Next season.
    OnTournamentStage.Broadcast(TournamentId, *State);
}

ETournamentState UTournamentSubsystem::GetTournamentState(FName TournamentId) const
{
    const ETournamentState* State = States.Find(TournamentId);
    return State ? *State : ETournamentState::Quiet;
}
