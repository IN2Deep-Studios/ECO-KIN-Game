// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "CompanionConditionTypes.generated.h"

// What can ail a working companion. Ailments are cared for, never
// punished: the game is about tending, not penalizing.
UENUM(BlueprintType)
enum class ECompanionAilment : uint8
{
    // Energy hit zero. Can't work until rested.
    Weary  UMETA(DisplayName = "Weary"),
    // Sick: from overwork while Weary, or blight exposure. Needs treatment.
    Ailing UMETA(DisplayName = "Ailing"),
    // Stressed: from raids, heavy strain, harsh conditions. Needs calming.
    Shaken UMETA(DisplayName = "Shaken")
};

// Where care comes from. Different sources suit different needs.
UENUM(BlueprintType)
enum class ECareSource : uint8
{
    Rest        UMETA(DisplayName = "Rest"),        // Mossrest Hollow, Bloomrest Loft, Elderrest
    Play        UMETA(DisplayName = "Play"),        // Frolic Field, Chime Grove, Splash Hollow
    Mender      UMETA(DisplayName = "Mender"),      // The Mender resident's hands
    Remedy      UMETA(DisplayName = "Remedy"),      // Remedy Garden brews
    Springs     UMETA(DisplayName = "Springs"),     // Soothing Springs
    Meal        UMETA(DisplayName = "Meal"),        // Campfire Care meals
    Counsel     UMETA(DisplayName = "Counsel"),     // Bondkeeper counsel, Harmony Circle
    Campfire    UMETA(DisplayName = "Campfire")     // Campfire rest scenes
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FCompanionCondition
{
    GENERATED_BODY()

    // 0-100. Work drains it; rest and play restore it. At 0, Weary sets in.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Care")
    float Energy = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Care")
    TArray<ECompanionAilment> Ailments;

    bool HasAilment(ECompanionAilment Ailment) const { return Ailments.Contains(Ailment); }
    bool IsFitToWork() const { return Energy > 0.0f && !Ailments.Contains(ECompanionAilment::Ailing); }
};
