// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "BaseBuilding/SharedStoresSubsystem.h"

void USharedStoresSubsystem::RegisterVault(FName VaultId)
{
    if (VaultId.IsNone() || Vaults.Contains(VaultId))
    {
        return;
    }
    Vaults.Add(VaultId, TMap<FName, int32>());
}

void USharedStoresSubsystem::DepositResource(FName VaultId, FName ResourceId, int32 Amount)
{
    TMap<FName, int32>* Vault = Vaults.Find(VaultId);
    if (!Vault || ResourceId.IsNone() || Amount <= 0)
    {
        return;
    }
    int32* Held = Vault->Find(ResourceId);
    Vault->Add(ResourceId, (Held ? *Held : 0) + Amount);
    OnStoresChanged.Broadcast(ResourceId, GetResourceTotal(ResourceId));
}

bool USharedStoresSubsystem::WithdrawResource(FName ResourceId, int32 Amount)
{
    if (ResourceId.IsNone() || Amount <= 0 || GetResourceTotal(ResourceId) < Amount)
    {
        return false;
    }
    int32 Remaining = Amount;
    for (auto& VaultPair : Vaults)
    {
        int32* Held = VaultPair.Value.Find(ResourceId);
        if (!Held || *Held <= 0)
        {
            continue;
        }
        const int32 Take = FMath::Min(*Held, Remaining);
        *Held -= Take;
        Remaining -= Take;
        if (Remaining <= 0)
        {
            break;
        }
    }
    OnStoresChanged.Broadcast(ResourceId, GetResourceTotal(ResourceId));
    return true;
}

int32 USharedStoresSubsystem::GetResourceTotal(FName ResourceId) const
{
    int32 Total = 0;
    for (const auto& VaultPair : Vaults)
    {
        const int32* Held = VaultPair.Value.Find(ResourceId);
        Total += Held ? *Held : 0;
    }
    return Total;
}
