// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TamerPlayerController.generated.h"

class UInputMappingContext;

// Owns input routing and camera intent for the Frequency Tamer. All gameplay
// input flows through Enhanced Input; this controller only maps context and
// exposes camera helpers (lock-on lives with the combat system).
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API ATamerPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ATamerPlayerController();

    // Current camera target for lock-on style framing (null = free camera).
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Camera")
    AActor* CameraFocusTarget = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Camera")
    void SetCameraFocus(AActor* NewTarget);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Camera")
    void ClearCameraFocus();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|Input")
    UInputMappingContext* TamerMappingContext;
};
