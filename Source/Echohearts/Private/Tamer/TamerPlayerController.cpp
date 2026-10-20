// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Tamer/TamerPlayerController.h"
#include "EnhancedInputSubsystems.h"

ATamerPlayerController::ATamerPlayerController()
{
    bShowMouseCursor = false;
}

void ATamerPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (TamerMappingContext)
        {
            Subsystem->AddMappingContext(TamerMappingContext, 0);
        }
    }
}

void ATamerPlayerController::SetCameraFocus(AActor* NewTarget)
{
    CameraFocusTarget = NewTarget;
}

void ATamerPlayerController::ClearCameraFocus()
{
    CameraFocusTarget = nullptr;
}
