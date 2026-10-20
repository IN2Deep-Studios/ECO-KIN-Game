// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Companion/WorkAptitudeTypes.h"
#include "WorkAptitudeSubsystem.generated.h"

// The species work table: which EcoKin are good at what, and how good.
// Aptitudes are innate (ecology), tiered 1-5, and deepen through Growth
// Rites — never through fusing duplicates, never through bought books.
// The workforce reads this table when assigning companions to jobs.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UWorkAptitudeSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Registers a species' innate work profile (data-driven; the full
    // Dex table fills in over time — this is the lookup, not the data).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Work")
    void RegisterSpeciesProfile(const FSpeciesWorkProfile& Profile);

    // Tier 1-5 for this species+aptitude, 0 if the species lacks it.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Work")
    int32 GetAptitudeTier(int32 DexID, EWorkAptitude Aptitude) const;

    // All aptitudes a species holds, with tiers.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Work")
    TMap<EWorkAptitude, int32> GetSpeciesAptitudes(int32 DexID) const;

    // A Growth Rite deepens one aptitude tier (max 5). The work deepens
    // as the EcoKin matures — this is our progression for aptitudes.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Work")
    bool DeepenAptitudeViaRite(int32 DexID, EWorkAptitude Aptitude);

    // Efficiency multiplier for a tier: 0.8 at tier 1, up to 1.6 at 5.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Work")
    static float EfficiencyForTier(int32 Tier);

private:
    UPROPERTY()
    TMap<int32, FSpeciesWorkProfile> Profiles;
};
