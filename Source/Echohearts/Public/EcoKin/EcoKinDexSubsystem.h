// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "EcoKinDexSubsystem.generated.h"

// Active Dex + Forms Registry runtime layout.
// CANON LOCKS ENFORCED HERE:
// - EcoKin are autonomous sentient partners, never inventory objects. There is no
//   consume / sacrifice / donor / genome-extraction API on this subsystem, and no
//   such function may be added (see ethical rules in 00_Canon_Lock).
// - Forms stay attached to their base EcoKin identity.
// - Squad contract: 8 carried / 3 active.
// - Market guard lives elsewhere; this subsystem never lists EcoKin for sale.
UCLASS()
class ECHOHEARTS_API UEcoKinDexSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Squad contract: 8 carried, 3 active.
    static constexpr int32 MaxCarriedSquadSize = 8;
    static constexpr int32 MaxActiveSquadSize = 3;

    // Permanent Production Dex: IDs 001-125 are the protected core roster.
    static constexpr int32 ProtectedCoreDexMax = 125;

    // Registers a permanent Dex identity. The Echoprint hash must be present and
    // is immutable afterwards. Seasonal additions use IDs above 125.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Dex")
    FEcoResult RegisterIdentity(const FEcoKinIdentity& Identity);

    // Attaches an Ecoform to its base EcoKin. Rejects forms whose base identity
    // is not registered, and rejects Shimmer Forms that do not preserve identity.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Dex")
    FEcoResult AttachForm(const FEcoformRecord& FormRecord);

    // Carried squad (max 8). Every ID must be a registered identity.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Dex")
    FEcoResult SetCarriedSquad(const TArray<int32>& EcoKinIDs);

    // Active squad (max 3). Every ID must be in the carried squad.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Dex")
    FEcoResult SetActiveSquad(const TArray<int32>& EcoKinIDs);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Dex")
    bool TryGetIdentity(int32 EcoKinID, FEcoKinIdentity& OutIdentity) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Dex")
    TArray<FEcoformRecord> GetFormsFor(int32 EcoKinID) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Dex")
    TArray<int32> GetCarriedSquad() const { return CarriedSquad; }

    UFUNCTION(BlueprintPure, Category = "Echohearts|Dex")
    TArray<int32> GetActiveSquad() const { return ActiveSquad; }

    UFUNCTION(BlueprintPure, Category = "Echohearts|Dex")
    static bool IsProtectedCoreID(int32 EcoKinID);

private:
    UPROPERTY()
    TMap<int32, FEcoKinIdentity> ActiveDex;

    UPROPERTY()
    TMap<int32, TArray<FEcoformRecord>> FormsRegistry;

    UPROPERTY()
    TArray<int32> CarriedSquad;

    UPROPERTY()
    TArray<int32> ActiveSquad;
};
