// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.

#include "Equipment/EquipmentSubsystem.h"

void UEquipmentSubsystem::RegisterEquipment(const FEquipmentDefinition& Def)
{
    if (Def.EquipId.IsNone() || Def.Tiers.Num() == 0)
    {
        return;
    }
    Registry.Add(Def.EquipId, Def);
}

bool UEquipmentSubsystem::EquipGear(FName EquipId, int32 TierIndex)
{
    const FEquipmentDefinition* Def = Registry.Find(EquipId);
    if (!Def || !Def->Tiers.IsValidIndex(TierIndex))
    {
        return false;
    }
    EquippedBySlot.Add(Def->Slot, EquipId);
    TierByEquip.Add(EquipId, TierIndex);
    OnGearEquipped.Broadcast(EquipId, TierIndex);
    return true;
}

void UEquipmentSubsystem::UnequipSlot(EEquipmentSlot Slot)
{
    if (const FName* EquipId = EquippedBySlot.Find(Slot))
    {
        TierByEquip.Remove(*EquipId);
        EquippedBySlot.Remove(Slot);
    }
}

bool UEquipmentSubsystem::UpgradeTier(FName EquipId)
{
    const FEquipmentDefinition* Def = Registry.Find(EquipId);
    const int32* TierIndex = TierByEquip.Find(EquipId);
    if (!Def || !TierIndex)
    {
        return false;
    }
    const int32 Next = *TierIndex + 1;
    if (!Def->Tiers.IsValidIndex(Next))
    {
        return false; // Already at max tier.
    }
    TierByEquip.Add(EquipId, Next);
    OnGearTierUpgraded.Broadcast(EquipId, Next);
    return true;
}

FEquipmentStatMods UEquipmentSubsystem::GetCombinedStatMods() const
{
    FEquipmentStatMods Combined;
    for (const auto& Pair : EquippedBySlot)
    {
        const FEquipmentDefinition* Def = Registry.Find(Pair.Value);
        const int32* TierIndex = TierByEquip.Find(Pair.Value);
        if (!Def || !TierIndex || !Def->Tiers.IsValidIndex(*TierIndex))
        {
            continue;
        }
        const FEquipmentStatMods& M = Def->Tiers[*TierIndex].StatMods;
        Combined.MoveSpeedMult *= M.MoveSpeedMult;
        Combined.DashDistanceMult *= M.DashDistanceMult;
        Combined.StaminaCostMult *= M.StaminaCostMult;
        Combined.EvasionWindowMult *= M.EvasionWindowMult;
        Combined.FallDamageMult *= M.FallDamageMult;
    }
    return Combined;
}

TArray<FEquipmentAbility> UEquipmentSubsystem::GetGrantedAbilities() const
{
    TArray<FEquipmentAbility> Out;
    for (const auto& Pair : EquippedBySlot)
    {
        const FEquipmentDefinition* Def = Registry.Find(Pair.Value);
        const int32* TierIndex = TierByEquip.Find(Pair.Value);
        if (!Def || !TierIndex || !Def->Tiers.IsValidIndex(*TierIndex))
        {
            continue;
        }
        const FEquipmentTier& Tier = Def->Tiers[*TierIndex];
        for (const FName& AbilityId : Tier.GrantedAbilities)
        {
            const FEquipmentAbility* Found = Def->Abilities.FindByPredicate(
                [&](const FEquipmentAbility& A) { return A.AbilityId == AbilityId; });
            if (Found)
            {
                Out.Add(*Found);
            }
        }
    }
    return Out;
}

bool UEquipmentSubsystem::IsEquipped(FName EquipId) const
{
    return TierByEquip.Contains(EquipId);
}

void UEquipmentSubsystem::RegisterSkystrideVelocityBoots()
{
    FEquipmentDefinition Def;
    Def.EquipId = FName("EQ-001");
    Def.DisplayName = FText::FromString("Skystride Velocity Boots");
    Def.Slot = EEquipmentSlot::Feet;
    Def.Rarity = FText::FromString("Rare -> Legendary");
    Def.EssenceSync = FName("Neutral");
    Def.ProgressionGate = FName("TamerProgression.SkyIslesUnlocked");
    Def.Obtainment = { FName("MainStory"), FName("ExplorationChests"),
                       FName("SpecialMissions"), FName("Crafting") };

    FEquipmentAbility VoltDash;
    VoltDash.AbilityId = FName("Skystride.VoltDash");
    VoltDash.DisplayName = FText::FromString("Volt Dash");
    VoltDash.Description = FText::FromString("Burst forward at high speed, leaving a short afterimage.");
    VoltDash.AnimaStrainCost = 8.0f;
    VoltDash.CooldownSeconds = 6.0f;

    FEquipmentAbility AerialStep;
    AerialStep.AbilityId = FName("Skystride.AerialStep");
    AerialStep.DisplayName = FText::FromString("Aerial Step");
    AerialStep.Description = FText::FromString("Perform a mid-air dash or direction change for greater mobility.");
    AerialStep.AnimaStrainCost = 12.0f;
    AerialStep.CooldownSeconds = 10.0f;

    FEquipmentAbility SwiftDodge;
    SwiftDodge.AbilityId = FName("Skystride.SwiftDodge");
    SwiftDodge.DisplayName = FText::FromString("Swift Dodge");
    SwiftDodge.Description = FText::FromString("Quick evasion with i-frames against incoming attacks.");
    SwiftDodge.AnimaStrainCost = 6.0f;
    SwiftDodge.CooldownSeconds = 4.0f;

    Def.Abilities = { VoltDash, AerialStep, SwiftDodge };

    auto MakeTier = [](const TCHAR* Name, float Move, float Dash, float Stam, float Evade, float Fall,
                       TArray<FName> Abilities) {
        FEquipmentTier T;
        T.TierId = FName(Name);
        T.DisplayName = FText::FromString(Name);
        T.StatMods.MoveSpeedMult = Move;
        T.StatMods.DashDistanceMult = Dash;
        T.StatMods.StaminaCostMult = Stam;
        T.StatMods.EvasionWindowMult = Evade;
        T.StatMods.FallDamageMult = Fall;
        T.GrantedAbilities = MoveTemp(Abilities);
        return T;
    };

    Def.Tiers.Add(MakeTier(TEXT("Skystride"), 1.10f, 1.15f, 0.95f, 1.10f, 0.80f,
        { FName("Skystride.VoltDash") }));
    Def.Tiers.Add(MakeTier(TEXT("Skystride+"), 1.15f, 1.25f, 0.90f, 1.18f, 0.70f,
        { FName("Skystride.VoltDash"), FName("Skystride.SwiftDodge") }));
    Def.Tiers.Add(MakeTier(TEXT("Apexstride"), 1.20f, 1.32f, 0.85f, 1.24f, 0.60f,
        { FName("Skystride.VoltDash"), FName("Skystride.SwiftDodge"), FName("Skystride.AerialStep") }));
    // NovaStride: full card stats (+25% / +40% / -20% / +30% / -50%).
    Def.Tiers.Add(MakeTier(TEXT("NovaStride"), 1.25f, 1.40f, 0.80f, 1.30f, 0.50f,
        { FName("Skystride.VoltDash"), FName("Skystride.SwiftDodge"), FName("Skystride.AerialStep") }));

    RegisterEquipment(Def);
}
