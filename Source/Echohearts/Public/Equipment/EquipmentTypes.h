// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipmentTypes.generated.h"

// Where a piece of gear is worn. Tamers only — EcoKin never wear equipment.
UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
    Head      UMETA(DisplayName = "Head"),
    Torso     UMETA(DisplayName = "Torso"),
    Hands     UMETA(DisplayName = "Hands"),
    Feet      UMETA(DisplayName = "Feet"),
    Accessory UMETA(DisplayName = "Accessory")
};

// Multiplicative stat modifiers from a gear tier. 1.0 = no change.
// StaminaCost/Cooldowns go DOWN (values < 1.0 are better); everything else UP.
USTRUCT(BlueprintType)
struct FEquipmentStatMods
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MoveSpeedMult = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float DashDistanceMult = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float StaminaCostMult = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float EvasionWindowMult = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float FallDamageMult = 1.0f;
};

// One upgrade tier of a gear piece (Skystride -> Skystride+ -> Apexstride -> NovaStride).
USTRUCT(BlueprintType)
struct FEquipmentTier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName TierId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
    // Shed/gifted crafting costs only — never a sacrificed partner, never a purchase gate.
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName, int32> UpgradeCosts;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FEquipmentStatMods StatMods;
    // Ability unlocks granted at this tier (by ability id).
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> GrantedAbilities;
};

// A gear ability: Volt Dash, Aerial Step, Swift Dodge, etc.
USTRUCT(BlueprintType)
struct FEquipmentAbility
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName AbilityId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Description;
    // Anima-Link strain cost per use — the limiter on high-speed evasion.
    // Zero strain means unlimited dodging, which we never allow.
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AnimaStrainCost = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float CooldownSeconds = 0.0f;
};

// Full definition of one equipment piece (registry row EQ-001, EQ-002, ...).
USTRUCT(BlueprintType)
struct FEquipmentDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName EquipId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) EEquipmentSlot Slot = EEquipmentSlot::Accessory;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Rarity;
    // Essence attunement: Neutral (all) or a specific essence from the nine.
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName EssenceSync = FName("Neutral");
    // Tamer progression gate (Kindling rank, story milestone, etc.) — never a raw "Level".
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName ProgressionGate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FEquipmentTier> Tiers;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FEquipmentAbility> Abilities;
    // Where it can be found: story, chests, missions, crafting.
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> Obtainment;
};
