// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EchoheartsGameMode.generated.h"

// Where the game is, moment to moment. Combat never ambushes exploration:
// entering combat is an explicit, telegraphed shift the player can read.
UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    Exploration UMETA(DisplayName = "Exploration"),
    Combat      UMETA(DisplayName = "Combat"),
    Ceremony    UMETA(DisplayName = "Ceremony"),   // Binding / Growth Rite / Release
    Dialogue    UMETA(DisplayName = "Dialogue"),
    Menu        UMETA(DisplayName = "Menu")
};

// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API AEchoheartsGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AEchoheartsGameMode();

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Game")
    EGamePhase CurrentPhase = EGamePhase::Exploration;

    // Phase transitions are explicit and broadcast. Ceremony phase locks
    // combat input so rites are never interrupted by a stray attack.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Game")
    void SetPhase(EGamePhase NewPhase);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, EGamePhase, NewPhase);

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Game")
    FOnPhaseChanged OnPhaseChanged;
};
