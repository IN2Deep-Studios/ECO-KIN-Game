// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Encounter/EncounterTypes.h"
#include "WildEcoKinSpawner.generated.h"

class AEcoKinCompanion;
class UBoxComponent;

// Placed in a region. Owns that region's wild population: rolls spawn entries
// by weight, spawns wild EcoKin inside its volume, enforces the population
// cap, and releases wanderers that stray too far from every tamer. One
// spawner per region is the intended setup.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-17).
UCLASS()
class ECHOHEARTS_API AWildEcoKinSpawner : public AActor
{
    GENERATED_BODY()

public:
    AWildEcoKinSpawner();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    FEcoSpawnTable SpawnTable;

    // The companion class to spawn. AEcoKinCompanion or a child Blueprint.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    TSubclassOf<AEcoKinCompanion> WildEcoKinClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echohearts|Encounter")
    UBoxComponent* SpawnVolume;

    // Dex IDs that may never roll the Alpha state (Nature is the canonical
    // example: never Alpha, never Omega). Data-driven, designer-owned.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Encounter")
    TArray<int32> NeverAlphaIDs;

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Encounter")
    int32 GetLivingWildCount() const;

    // Adds an exclusive species to this spawner's roll pool at runtime.
    // Called by UHiddenMissionSubsystem when a hidden mission completes:
    // the species joins the wild population of its habitat regions.
    // Duplicate Dex IDs are ignored; the entry validates through the Dex
    // subsystem like any other before spawning.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Encounter")
    void AddExclusiveSpecies(int32 EcoKinID, float Weight);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void SpawnTick();
    bool TrySpawnOne();
    const FEcoSpawnEntry* RollEntry() const;
    EEncounterState RollState(const FEcoSpawnEntry& Entry) const;
    FVector RollSpawnLocation() const;
    void PruneDistant();
    bool IsKnownSpecies(int32 EcoKinID) const;

    FTimerHandle SpawnTimerHandle;
    TArray<TWeakObjectPtr<AEcoKinCompanion>> LivingWild;
};
