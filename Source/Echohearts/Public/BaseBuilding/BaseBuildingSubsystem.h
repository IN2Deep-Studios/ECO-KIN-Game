// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BaseBuilding/BaseStructureTypes.h"
#include "BaseBuildingSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStructurePlaced, FName, StructureId, const FVector&, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStructureUpgraded, FName, StructureId, int32, NewTier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStructureDamaged, FName, StructureId, float, RemainingHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructureDestroyed, FName, StructureId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructureRepaired, FName, StructureId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructureRebuilt, FName, StructureId);

// Owns base building: structure registration, placement within the
// Heartroot Spire's radius, Deepen/Widen upgrades, and the defense rating
// that sanctuary raids test. Costs are data; the inventory system pays
// them by listening to OnStructurePlaced/OnStructureUpgraded.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UBaseBuildingSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Base")
    void RegisterStructure(const FBaseStructureDef& Def);

    // Places a structure. Must be within BuildRadiusCm of the Spire and the
    // structure must be registered. Returns false otherwise.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Base")
    bool PlaceStructure(FName StructureId, const FVector& Location);

    // Upgrades along Deepen (stronger) or Widen (broader). Tier caps at
    // the def's MaxTier.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Base")
    bool UpgradeStructure(FName StructureId, EUpgradePath Path);

    // Deals damage to a placed structure. At 0 HP it is destroyed: it
    // stops working and must be rebuilt. Returns true if destroyed.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Base")
    bool DamageStructure(FName StructureId, float Damage);

    // Repairs a damaged (not destroyed) structure to full HP. Broadcasts
    // the repair so the inventory system can charge a fraction of the
    // build cost. Destroyed structures can't be repaired — rebuild them.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Base")
    bool RepairStructure(FName StructureId);

    // Rebuilds a destroyed structure from scratch at full build cost.
    // Returns false if it wasn't destroyed.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Base")
    bool RebuildStructure(FName StructureId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Base")
    bool IsStructureDestroyed(FName StructureId) const;

    // IDs of placed structures still standing (not destroyed).
    UFUNCTION(BlueprintPure, Category = "Echohearts|Base")
    TArray<FName> GetStandingStructureIds() const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Base")
    bool IsStructurePlaced(FName StructureId) const;

    // Sum of placed defense structures' ratings, scaled by tier.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Base")
    float GetDefenseRating() const;

    // Strain recovery per hour from all placed Rest/Morale structures.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Base")
    float GetStrainRecoveryPerHour() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Base")
    float BuildRadiusCm = 4000.0f;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Base")
    FOnStructurePlaced OnStructurePlaced;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Base")
    FOnStructureUpgraded OnStructureUpgraded;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Base")
    FOnStructureDamaged OnStructureDamaged;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Base")
    FOnStructureDestroyed OnStructureDestroyed;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Base")
    FOnStructureRepaired OnStructureRepaired;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Base")
    FOnStructureRebuilt OnStructureRebuilt;

private:
    UPROPERTY()
    TMap<FName, FBaseStructureDef> Structures;

    UPROPERTY()
    TArray<FPlacedStructure> Placed;
};
