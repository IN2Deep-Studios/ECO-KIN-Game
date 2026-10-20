// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Tamer/TamerTypes.h"
#include "EcoKin/EcoKinKindling.h"
#include "Mission/MissionTypes.h"
#include "EchoheartsSaveGame.generated.h"

// One bonded partner's persisted state.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FSavedCompanion
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Echohearts|Save")
    int32 EcoKinID = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Echohearts|Save")
    float Health = 100.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Echohearts|Save")
    FKindlingState Kindling;
};

// Versioned save. SaveVersion increments on every schema change; older saves
// migrate forward, never silently dropped.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API UEchoheartsSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UEchoheartsSaveGame();

    // Bump this whenever the schema changes.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Save")
    int32 SaveVersion = 1;

    static constexpr int32 CurrentSaveVersion = 1;

    UPROPERTY(BlueprintReadWrite, Category = "Echohearts|Save")
    FTamerVitals TamerVitals;

    UPROPERTY(BlueprintReadWrite, Category = "Echohearts|Save")
    TArray<FSavedCompanion> Companions;

    // MissionId -> state, so the journal survives reload.
    UPROPERTY(BlueprintReadWrite, Category = "Echohearts|Save")
    TMap<FName, EMissionState> MissionStates;

    // RegionId -> visited / restored flags.
    UPROPERTY(BlueprintReadWrite, Category = "Echohearts|Save")
    TMap<FName, bool> RegionRestored;

    UPROPERTY(BlueprintReadWrite, Category = "Echohearts|Save")
    FName CurrentRegionId = NAME_None;

    UPROPERTY(BlueprintReadWrite, Category = "Echohearts|Save")
    FDateTime SavedAt;
};
