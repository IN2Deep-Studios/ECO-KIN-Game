// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Combat/EssenceAbilitySubsystem.h"
#include "AnimaLink/AnimaLinkCombatSubsystem.h"
#include "Companion/EcoKinCompanion.h"
#include "AnimaLink/AnimaLinkComponent.h"
#include "Engine/GameInstance.h"

FStrikeResult UEssenceAbilitySubsystem::ExecuteAbility(AEcoKinCompanion* Wielder, const FEssenceAbility& Ability, AActor* Target)
{
    FStrikeResult Result;

    if (!Wielder || !Target)
    {
        Result.FailureCode = FName(TEXT("Combat.Ability.NoWielderOrTarget"));
        return Result;
    }

    // The wielder must actually wield this essence.
    if (!Wielder->Essences.Contains(Ability.Essence))
    {
        Result.FailureCode = FName(TEXT("Combat.Ability.EssenceNotWielded"));
        return Result;
    }

    UAnimaLinkCombatSubsystem* Combat = ResolveCombat();
    if (!Combat)
    {
        Result.FailureCode = FName(TEXT("Combat.Ability.NoCombatSubsystem"));
        return Result;
    }

    // Strain is paid FIRST through the link. If the bond cannot bear it, the
    // ability fizzles — never silently, never for free.
    const FEcoResult StrainResult = Combat->ResolveTamerCommand(Wielder->AnimaLink, Ability.StrainCost);
    if (!StrainResult.bSucceeded)
    {
        Result.FailureCode = FName(TEXT("Combat.Ability.StrainRefused"));
        return Result;
    }

    // Only linked combatants take link damage. Anything else is not a valid
    // target for an essence strike.
    AEcoKinCompanion* TargetCompanion = Cast<AEcoKinCompanion>(Target);
    if (!TargetCompanion)
    {
        Result.FailureCode = FName(TEXT("Combat.Ability.InvalidTarget"));
        return Result;
    }

    float Damage = ScaleDamage(Ability, Wielder->Attributes);

    // Harmony precision: a guided strike lands true for bonus damage.
    const float HarmonyRoll = FMath::FRand();
    if (HarmonyRoll < Ability.PrecisionFactor * (Wielder->Attributes.Harmony / 100.0f))
    {
        Damage *= 1.5f;
        Result.bWasGuided = true;
    }

    TargetCompanion->TakeLinkDamage(Damage);

    Result.bLanded = true;
    Result.DamageDealt = Damage;
    return Result;
}

float UEssenceAbilitySubsystem::ScaleDamage(const FEssenceAbility& Ability, const FEcoKinCoreAttributes& Attr) const
{
    // Vibrance drives power; Density steadies the strike; Purity keeps it
    // clean (corrupted strikes lose force). All four attributes matter.
    const float PowerTerm = Ability.BasePower * (Attr.Vibrance / 50.0f);
    const float SteadyTerm = 1.0f + (Attr.Density - 50.0f) / 200.0f;
    const float PurityTerm = 0.75f + (Attr.Purity / 200.0f);
    return FMath::Max(1.0f, PowerTerm * SteadyTerm * PurityTerm);
}

UAnimaLinkCombatSubsystem* UEssenceAbilitySubsystem::ResolveCombat() const
{
    if (CombatOverride)
    {
        return CombatOverride;
    }
    if (const UGameInstance* GI = GetGameInstance())
    {
        return GI->GetSubsystem<UAnimaLinkCombatSubsystem>();
    }
    return nullptr;
}
