// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "CombatTypes.generated.h"

// One essence technique an Eco-Kin can wield. Damage is derived from the
// locked four-attribute matrix — never from generic ATK/DEF stats.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FEssenceAbility
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ability")
    FName AbilityId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ability")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ability")
    EEcoKinEssence Essence = EEcoKinEssence::Flora;

    // Base damage before attribute scaling.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ability")
    float BasePower = 10.0f;

    // Link strain this ability costs. Paid through the Anima-Link combat
    // subsystem; the ability fizzles if the link cannot bear it.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ability")
    float StrainCost = 8.0f;

    // 0-1. Higher values let Harmony guide the strike (crit-like precision).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ability")
    float PrecisionFactor = 0.1f;
};

// A resolved strike: who hit whom, with what, for how much.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FStrikeResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Combat")
    bool bLanded = false;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Combat")
    float DamageDealt = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Combat")
    bool bWasGuided = false; // Harmony precision struck true

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Combat")
    FName FailureCode = NAME_None;
};
