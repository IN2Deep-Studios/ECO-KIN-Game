// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Server/SessionSubsystem.h"

bool USessionSubsystem::CreateSession(EServerType ServerType, const FString& ShardId, int32 MaxTamers, FGuid& OutSessionId)
{
    OutSessionId = FGuid::NewGuid();
    FServerSession Session;
    Session.SessionId = OutSessionId;
    Session.ServerType = ServerType;
    Session.ShardId = ShardId;
    Session.MaxTamers = FMath::Max(1, MaxTamers);
    Session.CurrentTamers = 1;
    Session.State = ESessionState::Idle;
    Sessions.Add(OutSessionId, Session);
    CurrentSessionId = OutSessionId;
    // SCAFFOLD: local record only. No server process is contacted.
    return true;
}

bool USessionSubsystem::FindSession(const FMatchmakingRequest& Request, FGuid& OutSessionId)
{
    // SCAFFOLD: match against the local registry. Real matchmaking will
    // query the shard directory; the request shape already carries
    // everything it will need (mode, server type, shard, trust band).
    for (const TPair<FGuid, FServerSession>& Pair : Sessions)
    {
        const FServerSession& S = Pair.Value;
        if (S.ServerType != Request.PreferredServer)
        {
            continue;
        }
        if (!Request.PreferredShard.IsEmpty() && S.ShardId != Request.PreferredShard)
        {
            continue;
        }
        if (S.CurrentTamers >= S.MaxTamers)
        {
            continue;
        }
        if (Pair.Key == CurrentSessionId)
        {
            continue;
        }
        OutSessionId = Pair.Key;
        return true;
    }
    return false;
}

bool USessionSubsystem::JoinSession(const FGuid& SessionId)
{
    FServerSession* Session = Sessions.Find(SessionId);
    if (!Session || Session->CurrentTamers >= Session->MaxTamers)
    {
        return false;
    }
    Session->CurrentTamers++;
    CurrentSessionId = SessionId;
    // SCAFFOLD: no handshake, no attestation yet. The anti-cheat
    // session-attestation module will gate this join when netcode lands.
    return true;
}

bool USessionSubsystem::LeaveSession()
{
    if (!CurrentSessionId.IsValid())
    {
        return false;
    }
    if (FServerSession* Session = Sessions.Find(CurrentSessionId))
    {
        Session->CurrentTamers = FMath::Max(0, Session->CurrentTamers - 1);
    }
    CurrentSessionId.Invalidate();
    return true;
}

bool USessionSubsystem::TryGetCurrentSession(FServerSession& OutSession) const
{
    const FServerSession* Session = Sessions.Find(CurrentSessionId);
    if (!Session)
    {
        return false;
    }
    OutSession = *Session;
    return true;
}
