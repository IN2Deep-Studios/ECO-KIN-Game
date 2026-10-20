// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Mutation/MutationTypes.h"
#include "MutationDirector.generated.h"

// Directs mutation requests into real Mutation FORMS on the Forms Registry.
// Forms stay attached to their base EcoKin identity (Dex subsystem rule).
// CANON GUARD: Nature (Legendary Humanoid-Kin) allows CONDITIONAL Mutations
// only — unconditional mutation requests against a registered Nature identity
// are refused here, explicitly, every time.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-14).
UCLASS()
class ECHOHEARTS_API UMutationDirector : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Registers a Nature (Legendary Humanoid-Kin) base identity. Mutations
    // against these IDs are conditional-only, per canon lock.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Mutation")
    void RegisterNatureIdentity(int32 EcoKinID);

    // Test seam: override the Dex subsystem (defaults to the game instance's).
    void SetDexSubsystemOverride(class UEcoKinDexSubsystem* InDex) { DexOverride = InDex; }

    // Canon query: is this a registered Nature (Legendary Humanoid-Kin) identity?
    // Growth Rites for these identities route through conditional-only Mutation.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Mutation")
    bool IsNatureIdentity(int32 EcoKinID) const { return NatureIdentityIDs.Contains(EcoKinID); }

    // Requests a mutation. On success, OutForm is a Mutation-kind FEcoformRecord
    // attached to the target's base identity in the Forms Registry.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Mutation")
    FEcoResult RequestMutation(const FMutationTrigger& Trigger, FEcoformRecord& OutForm);

private:
    class UEcoKinDexSubsystem* ResolveDex() const;

    UPROPERTY()
    TSet<int32> NatureIdentityIDs;

    UPROPERTY()
    class UEcoKinDexSubsystem* DexOverride = nullptr;
};
