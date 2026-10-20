// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimaLink/AnimaLinkTypes.h"
#include "AnimaLinkComponent.generated.h"

// Bidirectional Anima-Link contract between a tamer and a bonded Eco-Kin.
// The link carries strain BOTH ways: Eco-Kin damage/strain feeds back onto the
// tamer's stamina, and tamer-side overexertion strains the link in return.
// This component enforces the accounting; gameplay systems consume the numbers.
UCLASS(ClassGroup = (Echohearts), meta = (BlueprintSpawnableComponent))
class ECHOHEARTS_API UAnimaLinkComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAnimaLinkComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AnimaLink")
    FAnimaLinkState LinkState;

    // Opens the link. Fails if already active.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|AnimaLink")
    FEcoResult BeginLink();

    // Closes the link and clears accumulated strain. Release is always allowed;
    // a companion is a relationship, not a container.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|AnimaLink")
    FEcoResult EndLink();

    // Eco-Kin-side event: the bonded Eco-Kin took strain/damage.
    // Returns the stamina feedback to apply to the tamer (bidirectional link).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|AnimaLink")
    float NotifyEcoKinStrain(float StrainAmount);

    // Tamer-side event: the tamer overexerted. Strains the link in return.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|AnimaLink")
    float NotifyTamerOverexertion(float ExertionAmount);

    // Transformation cost gate: Growth Rite / Mutation / Shimmer Form attempts
    // must pay Link Strain first. Returns false when the link cannot bear it.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|AnimaLink")
    bool TryPayTransformationCost(float StrainCost);
};
