// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "AnimaLinkCombatSubsystem.generated.h"

// Anima-Link COMBAT orchestration. The link itself is bidirectional by canon:
// Eco-Kin strain/damage feeds back as stamina/health consequences on the tamer,
// and tamer overexertion strains the link in return. This subsystem routes
// combat exchanges through UAnimaLinkComponent so no combat path can bypass
// the feedback accounting. There is no one-sided damage model in Echohearts.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API UAnimaLinkCombatSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Eco-Kin-side hit: the bonded Eco-Kin takes DamageAmount in combat.
    // Routes through the link; OutTamerStaminaFeedback is the stamina cost the
    // tamer pays for their partner's pain. Refuses the exchange when it would
    // fracture the link (Shadow-fracture condition), rather than silently
    // breaking the bond.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|AnimaLink|Combat")
    FEcoResult ResolveEcoKinHit(class UAnimaLinkComponent* Link, float DamageAmount, float& OutTamerStaminaFeedback);

    // Tamer-side command: the tamer orders a combat action costing link strain.
    // Refused when the link cannot bear the cost — the tamer must rest or
    // restore the bond first. Commands never bypass strain.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|AnimaLink|Combat")
    FEcoResult ResolveTamerCommand(class UAnimaLinkComponent* Link, float CommandStrainCost);

    // Shared guard: is this link fit to keep fighting?
    UFUNCTION(BlueprintPure, Category = "Echohearts|AnimaLink|Combat")
    bool IsLinkFitForCombat(const class UAnimaLinkComponent* Link) const;
};
