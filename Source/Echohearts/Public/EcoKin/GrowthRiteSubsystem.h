// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "EcoKin/EcoKinKindling.h"
#include "GrowthRiteSubsystem.generated.h"

// GROWTH RITE execution. Growth Rites REPLACE evolutions: structured maturation
// with no level resets, gated by Biological Maturity, Kindling (trust %),
// Ecology accomplishments, and Catalyst items. The Echoprint is preserved across
// every rite — an approved identity is never silently replaced.
//
// CANON GUARDS ENFORCED HERE:
// - Nature (Legendary Humanoid-Kin) takes CONDITIONAL Mutations only; rite
//   requests for a registered Nature identity route through the Mutation
//   Director, which refuses unconditional mutation every time.
// - There is no sacrifice path. No function here consumes, destroys, or trades
//   away a sentient partner; willing Release is handled by the Release
//   subsystem and is never a rite cost.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API UGrowthRiteSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Test seam: override the Dex subsystem (defaults to the game instance's).
    void SetDexSubsystemOverride(class UEcoKinDexSubsystem* InDex) { DexOverride = InDex; }

    // Test seam: override the Mutation Director (defaults to the game instance's).
    void SetMutationDirectorOverride(class UMutationDirector* InDirector) { MutationOverride = InDirector; }

    // Performs a Growth Rite for a bonded Eco-Kin.
    // Gates (all required): Kindling.CanPerformGrowthRite(RequiredAccomplishments).
    // The rite pays LinkStrainCost through the Anima-Link first; the link must
    // bear the transformation. On success, OutForm is the new Ecoform attached
    // to the Eco-Kin's base identity with its Echoprint verified unchanged.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|GrowthRite")
    FEcoResult RequestGrowthRite(int32 EcoKinID, FKindlingState& Kindling, class UAnimaLinkComponent* Link,
        int32 RequiredAccomplishments, float RiteStrainCost, FEcoformRecord& OutForm);

private:
    class UEcoKinDexSubsystem* ResolveDex() const;
    class UMutationDirector* ResolveMutationDirector() const;

    UPROPERTY()
    class UEcoKinDexSubsystem* DexOverride = nullptr;

    UPROPERTY()
    class UMutationDirector* MutationOverride = nullptr;
};
