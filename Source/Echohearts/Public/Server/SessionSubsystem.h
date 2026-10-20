// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Server/SessionTypes.h"
#include "SessionSubsystem.generated.h"

// SESSION & MATCHMAKING — SCAFFOLD.
//
// This subsystem defines the SHAPE of multiplayer: session lifecycle,
// matchmaking requests, server types. It is honest scaffolding — the maps,
// state machines, and flow are real, but NO NETWORKING is implemented here.
// No sockets, no replication, no dedicated-server handshake. When netcode
// arrives, it plugs into this interface; until then, everything resolves
// locally (solo Tamer + AI allies).
//
// Server-authoritative combat, the anti-cheat session-attestation module,
// and UE replication will converge here. See the server architecture doc
// for the full design.
// SCAFFOLD / NOT IMPLEMENTED (2026-10-17).
UCLASS()
class ECHOHEARTS_API USessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Creates a local session record (no network).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Server")
    bool CreateSession(EServerType ServerType, const FString& ShardId, int32 MaxTamers, FGuid& OutSessionId);

    // Finds a session matching the request (local registry for now).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Server")
    bool FindSession(const FMatchmakingRequest& Request, FGuid& OutSessionId);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Server")
    bool JoinSession(const FGuid& SessionId);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Server")
    bool LeaveSession();

    UFUNCTION(BlueprintPure, Category = "Echohearts|Server")
    bool IsInSession() const { return CurrentSessionId.IsValid(); }

    UFUNCTION(BlueprintPure, Category = "Echohearts|Server")
    bool TryGetCurrentSession(FServerSession& OutSession) const;

private:
    UPROPERTY()
    TMap<FGuid, FServerSession> Sessions;

    FGuid CurrentSessionId;
};
