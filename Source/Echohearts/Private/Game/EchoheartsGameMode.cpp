// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Game/EchoheartsGameMode.h"
#include "Tamer/FrequencyTamerCharacter.h"
#include "Tamer/TamerPlayerController.h"
#include "UI/EchoheartsHUD.h"

AEchoheartsGameMode::AEchoheartsGameMode()
{
    DefaultPawnClass = AFrequencyTamerCharacter::StaticClass();
    PlayerControllerClass = ATamerPlayerController::StaticClass();
    HUDClass = AEchoheartsHUD::StaticClass();
}

void AEchoheartsGameMode::SetPhase(EGamePhase NewPhase)
{
    if (NewPhase == CurrentPhase)
    {
        return;
    }
    CurrentPhase = NewPhase;
    OnPhaseChanged.Broadcast(NewPhase);
}
