// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Equipment/EquipmentTypes.h"
#include "EquipmentSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGearEquipped, FName, EquipId, int32, TierIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGearTierUpgraded, FName, EquipId, int32, NewTierIndex);

// Owns the Tamer equipment registry: gear definitions, equipped pieces per
// slot, and tier upgrades. Gear is TAMER equipment only — EcoKin never wear
// gear, and gear never gates main progression by itself.
//
// Design rules (canon):
// - Stat mods are multiplicative and interact with the four attributes
//   (Vibrance, Density, Harmony, Purity), never with ATK/HP/SPD/DEF.
// - Every gear ability that moves the Tamer fast costs Anima-Link strain —
//   no unlimited high-speed evasion, ever.
// - Upgrade costs are shed/gifted crafting resources only. No sacrificed
//   partners, no purchase gates, no new currencies.
// - Progression gates use Tamer progression terms (Kindling rank, story
//   milestone), never a raw "Level".
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UEquipmentSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Registers a gear definition (registry rows EQ-001, EQ-002, ...).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Equipment")
    void RegisterEquipment(const FEquipmentDefinition& Def);

    // Equips a registered piece into its slot at the given tier index.
    // Returns false if the piece isn't registered or the tier is invalid.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Equipment")
    bool EquipGear(FName EquipId, int32 TierIndex);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Equipment")
    void UnequipSlot(EEquipmentSlot Slot);

    // Upgrades an equipped piece to its next tier. The caller (crafting UI)
    // is responsible for spending the shed/gifted costs listed on the tier.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Equipment")
    bool UpgradeTier(FName EquipId);

    // Combined stat mods across all equipped gear (multiplicative stacking).
    UFUNCTION(BlueprintPure, Category = "Echohearts|Equipment")
    FEquipmentStatMods GetCombinedStatMods() const;

    // All gear abilities currently granted by equipped tiers.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Equipment")
    TArray<FEquipmentAbility> GetGrantedAbilities() const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Equipment")
    bool IsEquipped(FName EquipId) const;

    // Builds and registers EQ-001 Skystride Velocity Boots with all four
    // tiers (Skystride / Skystride+ / Apexstride / NovaStride) and its three
    // abilities (Volt Dash / Aerial Step / Swift Dodge).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Equipment")
    void RegisterSkystrideVelocityBoots();

protected:
    UPROPERTY() TMap<FName, FEquipmentDefinition> Registry;
    // Slot -> (EquipId, TierIndex)
    UPROPERTY() TMap<EEquipmentSlot, FName> EquippedBySlot;
    UPROPERTY() TMap<FName, int32> TierByEquip;
};
