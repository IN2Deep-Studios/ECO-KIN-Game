// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SharedStoresSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStoresChanged, FName, ResourceId, int32, NewTotal);

// The Shared Stores: the base breathes as one storehouse. Vaults linked
// (the Underroot Vault's Widen path), Swiftcarry companions hauling —
// crafting pulls from the whole base, no chest-running. Shed and gifted
// resources, counted honestly.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API USharedStoresSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Base")
    void RegisterVault(FName VaultId);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Base")
    void DepositResource(FName VaultId, FName ResourceId, int32 Amount);

    // Pulls across all vaults, oldest deposits first. Returns false if
    // the base doesn't hold enough — no silent creation.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Base")
    bool WithdrawResource(FName ResourceId, int32 Amount);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Base")
    int32 GetResourceTotal(FName ResourceId) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Base")
    FOnStoresChanged OnStoresChanged;

private:
    // VaultId -> (ResourceId -> amount)
    UPROPERTY()
    TMap<FName, TMap<FName, int32>> Vaults;
};
