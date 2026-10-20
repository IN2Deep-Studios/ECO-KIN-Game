// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "CampfireTypes.generated.h"

// A meal: foraged ingredients in, care out. Ingredient keys are gathered
// resource IDs — the subsystem rejects anything not foraged, because the
// no-sacrifice rule reaches the kitchen.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FMealDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FName MealId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText Description;

    // Resource ID -> units required. All must be foraged/gathered.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    TMap<FName, int32> RequiredIngredients;

    // Kindling trust gained per partner sharing the meal.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    float TrustGain = 2.0f;

    // Link strain recovered per partner sharing the meal.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    float StrainRecovery = 10.0f;

    // True for calming meals (Hollow-warding rest).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    bool bCalming = false;
};
