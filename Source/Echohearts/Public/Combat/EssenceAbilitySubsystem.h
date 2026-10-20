// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Combat/CombatTypes.h"
#include "EssenceAbilitySubsystem.generated.h"

// Executes essence abilities in combat. Every strike flows through the
// Anima-Link: the ability's strain is paid first, damage is scaled by the
// four locked attributes, and feedback reaches the tamer on the return path.
// There is no damage number that bypasses the bond.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API UEssenceAbilitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Test seam: override the combat subsystem (defaults to game instance's).
    void SetCombatSubsystemOverride(class UAnimaLinkCombatSubsystem* InCombat) { CombatOverride = InCombat; }

    // The bonded Eco-Kin uses Ability against Target. AttributeWielder carries
    // the attacker's Vibrance/Density/Harmony/Purity. Target must expose
    // TakeLinkDamage (AEcoKinCompanion) or the strike is refused.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Combat")
    FStrikeResult ExecuteAbility(class AEcoKinCompanion* Wielder, const FEssenceAbility& Ability, AActor* Target);

private:
    float ScaleDamage(const FEssenceAbility& Ability, const struct FEcoKinCoreAttributes& Attr) const;
    class UAnimaLinkCombatSubsystem* ResolveCombat() const;

    UPROPERTY()
    class UAnimaLinkCombatSubsystem* CombatOverride = nullptr;
};
