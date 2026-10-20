// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SessionTypes.generated.h"

// Server types. Where you play shapes what the world allows — never who
// you are as a Tamer. A Tamer's bonds travel with them across servers.
UENUM(BlueprintType)
enum class EServerType : uint8
{
    // Cooperative PvE. Tamers restore the world together; duels only by
    // mutual consent (Sparring). The default home.
    Sanctuary   UMETA(DisplayName = "Sanctuary"),
    // Consensual competitive play. Ranked-style Duel ladders run here;
    // every duel still requires both Tamers' agreement. Nothing forced.
    Contested   UMETA(DisplayName = "Contested")
};

UENUM(BlueprintType)
enum class ESessionState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Matchmaking UMETA(DisplayName = "Matchmaking"),
    InBattle    UMETA(DisplayName = "InBattle"),
    InRaid      UMETA(DisplayName = "InRaid"),
    Leaving     UMETA(DisplayName = "Leaving")
};

// One hosted session: a shard of Rebearth holding a set of Tamers.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FServerSession
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Server")
    FGuid SessionId;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Server")
    EServerType ServerType = EServerType::Sanctuary;

    // Regional shard, e.g. "Ashfall", "MistCoasts". Shards keep latency
    // low and let regions feel lived-in.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Server")
    FString ShardId;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Server")
    int32 MaxTamers = 32;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Server")
    int32 CurrentTamers = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Server")
    ESessionState State = ESessionState::Idle;
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FMatchmakingRequest
{
    GENERATED_BODY()

    // Duel, Raid, or Sparring — what the Tamer is looking for.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Server")
    FString BattleMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Server")
    EServerType PreferredServer = EServerType::Sanctuary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Server")
    FString PreferredShard;

    // Kindling-aware matchmaking: sparring partners are matched by trust
    // band so novices meet novices and veterans meet veterans.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Server")
    float TrustBandMin = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Server")
    float TrustBandMax = 100.0f;
};
