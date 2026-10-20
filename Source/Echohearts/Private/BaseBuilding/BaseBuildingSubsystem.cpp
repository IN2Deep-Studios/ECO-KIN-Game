// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "BaseBuilding/BaseBuildingSubsystem.h"

void UBaseBuildingSubsystem::RegisterStructure(const FBaseStructureDef& Def)
{
    if (Def.StructureId.IsNone() || Def.MaxTier < 1)
    {
        return;
    }
    Structures.Add(Def.StructureId, Def);
}

bool UBaseBuildingSubsystem::PlaceStructure(FName StructureId, const FVector& Location)
{
    const FBaseStructureDef* Def = Structures.Find(StructureId);
    if (!Def)
    {
        return false;
    }
    // Everything builds within the Heartroot Spire's radius. The Spire
    // itself is placed first at the origin of the base.
    if (Location.Size() > BuildRadiusCm)
    {
        return false;
    }
    for (const FPlacedStructure& P : Placed)
    {
        if (P.StructureId == StructureId)
        {
            return false; // One of each per base. Deepen/Widen instead.
        }
    }

    FPlacedStructure PlacedOne;
    PlacedOne.StructureId = StructureId;
    PlacedOne.Tier = 1;
    PlacedOne.Location = Location;
    PlacedOne.CurrentHitPoints = Def->MaxHitPoints;
    PlacedOne.bDestroyed = false;
    Placed.Add(PlacedOne);

    OnStructurePlaced.Broadcast(StructureId, Location);
    return true;
}

bool UBaseBuildingSubsystem::UpgradeStructure(FName StructureId, EUpgradePath Path)
{
    const FBaseStructureDef* Def = Structures.Find(StructureId);
    if (!Def)
    {
        return false;
    }
    for (FPlacedStructure& P : Placed)
    {
        if (P.StructureId == StructureId)
        {
            if (P.Tier >= Def->MaxTier)
            {
                return false;
            }
            ++P.Tier;
            OnStructureUpgraded.Broadcast(StructureId, P.Tier);
            return true;
        }
    }
    return false;
}

float UBaseBuildingSubsystem::GetDefenseRating() const
{
    float Rating = 0.0f;
    for (const FPlacedStructure& P : Placed)
    {
        // Rubble defends nothing.
        if (P.bDestroyed)
        {
            continue;
        }
        const FBaseStructureDef* Def = Structures.Find(P.StructureId);
        if (Def && Def->Category == EStructureCategory::Defense)
        {
            Rating += Def->DefenseRating * static_cast<float>(P.Tier);
        }
    }
    return Rating;
}

float UBaseBuildingSubsystem::GetStrainRecoveryPerHour() const
{
    float Recovery = 0.0f;
    for (const FPlacedStructure& P : Placed)
    {
        if (P.bDestroyed)
        {
            continue;
        }
        const FBaseStructureDef* Def = Structures.Find(P.StructureId);
        if (Def && (Def->Category == EStructureCategory::Rest || Def->Category == EStructureCategory::Morale))
        {
            Recovery += Def->StrainRecoveryPerHour * static_cast<float>(P.Tier);
        }
    }
    return Recovery;
}

bool UBaseBuildingSubsystem::DamageStructure(FName StructureId, float Damage)
{
    if (Damage <= 0.0f)
    {
        return false;
    }
    for (FPlacedStructure& P : Placed)
    {
        if (P.StructureId == StructureId && !P.bDestroyed)
        {
            P.CurrentHitPoints = FMath::Max(0.0f, P.CurrentHitPoints - Damage);
            if (P.CurrentHitPoints <= 0.0f)
            {
                P.bDestroyed = true;
                OnStructureDestroyed.Broadcast(StructureId);
                return true;
            }
            OnStructureDamaged.Broadcast(StructureId, P.CurrentHitPoints);
            return false;
        }
    }
    return false;
}

bool UBaseBuildingSubsystem::RepairStructure(FName StructureId)
{
    const FBaseStructureDef* Def = Structures.Find(StructureId);
    if (!Def)
    {
        return false;
    }
    for (FPlacedStructure& P : Placed)
    {
        if (P.StructureId == StructureId && !P.bDestroyed && P.CurrentHitPoints < Def->MaxHitPoints)
        {
            P.CurrentHitPoints = Def->MaxHitPoints;
            // The inventory system charges a fraction of the build cost
            // on this broadcast.
            OnStructureRepaired.Broadcast(StructureId);
            return true;
        }
    }
    return false;
}

bool UBaseBuildingSubsystem::RebuildStructure(FName StructureId)
{
    const FBaseStructureDef* Def = Structures.Find(StructureId);
    if (!Def)
    {
        return false;
    }
    for (FPlacedStructure& P : Placed)
    {
        if (P.StructureId == StructureId && P.bDestroyed)
        {
            P.bDestroyed = false;
            P.CurrentHitPoints = Def->MaxHitPoints;
            P.Tier = 1; // Rebuilt from scratch — tiers are earned again.
            // The inventory system charges the full build cost on this.
            OnStructureRebuilt.Broadcast(StructureId);
            return true;
        }
    }
    return false;
}

bool UBaseBuildingSubsystem::IsStructureDestroyed(FName StructureId) const
{
    for (const FPlacedStructure& P : Placed)
    {
        if (P.StructureId == StructureId)
        {
            return P.bDestroyed;
        }
    }
    return false;
}

TArray<FName> UBaseBuildingSubsystem::GetStandingStructureIds() const
{
    TArray<FName> Ids;
    for (const FPlacedStructure& P : Placed)
    {
        if (!P.bDestroyed)
        {
            Ids.Add(P.StructureId);
        }
    }
    return Ids;
}

bool UBaseBuildingSubsystem::IsStructurePlaced(FName StructureId) const
{
    for (const FPlacedStructure& P : Placed)
    {
        if (P.StructureId == StructureId)
        {
            return true;
        }
    }
    return false;
}
