// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TideSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTideWindow, bool, bLowTide);

// Tidecaller's Paths: low-tide secrets — underwater caves, drowned
// shrines, the paths the sea only loans twice a day. The Tidecaller's
// Bell rings true when the window opens.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UTideSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void BeginTideWatch(float FullCycleSeconds);

    // Called by the game clock; opens/closes the low-tide window.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportTidePhase(bool bLowTide);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void ReportSecretFound(FName SecretId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Adventure")
    bool IsLowTide() const { return bIsLowTide; }

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnTideWindow OnTideWindow;

private:
    UPROPERTY()
    bool bIsLowTide = false;

    UPROPERTY()
    TSet<FName> FoundSecrets;
};
