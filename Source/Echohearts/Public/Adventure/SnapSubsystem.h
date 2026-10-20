// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SnapSubsystem.generated.h"

// What the lens can catch.
UENUM(BlueprintType)
enum class EPhotoSubject : uint8
{
    Companion   UMETA(DisplayName = "Companion"),
    WildEcoKin  UMETA(DisplayName = "Wild EcoKin"),
    Vista       UMETA(DisplayName = "Vista"),
    Wonder      UMETA(DisplayName = "Wonder"),      // rare phenomena
    Rite        UMETA(DisplayName = "Rite"),        // ceremonies mid-motion
    Storm       UMETA(DisplayName = "Storm")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPhotoCaptured, EPhotoSubject, Subject, bool, bFirstOfKind);

// A.E.G.I.S. Snap: photo mode. Catalog wonders, rare behaviors, and
// perfect moments — the Field Journal's visual memory. Rare subjects
// pay Kindling and accomplishments.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API USnapSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void CapturePhoto(EPhotoSubject Subject, FName SubjectId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Adventure")
    int32 GetCatalogCount() const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnPhotoCaptured OnPhotoCaptured;

private:
    UPROPERTY()
    TSet<FName> CatalogedSubjects;
};
