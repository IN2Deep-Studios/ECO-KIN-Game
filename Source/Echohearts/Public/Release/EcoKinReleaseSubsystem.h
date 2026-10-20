// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Release/ReleaseHarvestTypes.h"
#include "EcoKinReleaseSubsystem.generated.h"

// Willing-release harvesting and world flora gathering.
// CANON LOCK — RELEASE IS NOT SACRIFICE:
// A duplicate EcoKin that willingly departs leaves SHED/GIFTED bodily resources
// (molted scales, shed feathers, fallen petals, sap, spores, crystal shards,
// essence traces). Nothing is killed, harmed, or consumed. Sacrifice —
// destroying a sentient partner for power — is FORBIDDEN. There is deliberately
// NO API on this subsystem that destroys an EcoKin for resources outside the
// willing-release flow, and none may be added.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-14).
UCLASS()
class ECHOHEARTS_API UEcoKinReleaseSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Releases a duplicate EcoKin. The release MUST be willing (bWilling=true);
    // an unwilling release is refused with an empty yield. The EcoKin itself is
    // never destroyed by this call — release returns it to the wild.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Release")
    FEcoResult ReleaseEcoKin(const FEcoKinIdentity& Identity, bool bWilling, FReleaseYield& OutYield);

    // Registers a harvestable world flora node.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Release")
    FEcoResult RegisterFloraNode(const FHarvestNode& Node);

    // Gathers from a node. Fails while the node is still regrowing (respawn).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Release")
    FEcoResult HarvestNode(FName NodeId, FReleaseYield& OutYield);

    // All-time star-tier progress for one resource ID.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Release")
    int32 GetLifetimePoints(FName ResourceId) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Release")
    EResourceStarTier GetStarTier(FName ResourceId) const;

private:
    // Builds the order-appropriate shed/gifted resource table for one identity.
    void BuildReleaseTable(const FEcoKinIdentity& Identity, FReleaseYield& OutYield) const;

    // Essence-flavored adjective, e.g. "Pyre-Scorched Scale".
    static FString EssenceAdjective(EEcoKinEssence Essence);

    // Stamps a stack with lifetime progress and adds points at the given rate.
    void StampStack(FResourceStack& Stack, float PointsPerUnit);

    UPROPERTY()
    TMap<FName, int32> LifetimePointsByResource;

    UPROPERTY()
    TMap<FName, FHarvestNode> FloraNodes;

    UPROPERTY()
    TMap<FName, double> LastHarvestSecondsByNode;
};
