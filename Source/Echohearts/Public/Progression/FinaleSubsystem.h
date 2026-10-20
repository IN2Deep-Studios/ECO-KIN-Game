// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FinaleSubsystem.generated.h"

// The endgame sequence, using pieces that already exist.
UENUM(BlueprintType)
enum class EFinaleStage : uint8
{
    Dormant       UMETA(DisplayName = "Dormant"),
    Assembling    UMETA(DisplayName = "Assembling"),    // the Elderwing is readied
    Voyage        UMETA(DisplayName = "Voyage"),        // flight into the storm
    EyeOfStorm    UMETA(DisplayName = "Eye of the Storm"),
    Purification  UMETA(DisplayName = "Purification"),  // Shadow Beast Prime: hold the circle
    RestorationEra UMETA(DisplayName = "Restoration Era")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFinaleStage, EFinaleStage, Stage);

// The Finale: the definitive endgame, tracked. The Elderwing is
// readied, flown into the eye of the mainland's corruption storm,
// and there — in the longest Rite Defense ever held — Shadow Beast
// Prime is walked back, not slain. Then: the Restoration Era. The
// world breathes easier, and it stays breathed.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UFinaleSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Opens only when the game says the world is ready (the 14-boss
    // road walked, the societies answered).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Finale")
    void SetGateOpen(bool bOpen);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Finale")
    bool BeginFinale();

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Finale")
    void ReportStageComplete(EFinaleStage Stage);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Finale")
    EFinaleStage GetCurrentStage() const { return CurrentStage; }

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Finale")
    FOnFinaleStage OnFinaleStage;

private:
    UPROPERTY()
    EFinaleStage CurrentStage = EFinaleStage::Dormant;

    UPROPERTY()
    bool bGateOpen = false;
};
