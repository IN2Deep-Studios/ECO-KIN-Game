// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TriumphSubsystem.generated.h"

// One boss's triumph: what its fall unlocks at home.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FBossTriumph
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Triumph")
    int32 BossDexID = 0;

    // Structure IDs (our 23+, our crafting tiers) this triumph unlocks.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Triumph")
    TArray<FName> UnlockedStructureIds;

    // The proclamation the Sanctuary hears — voiced, never a debug line.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Triumph")
    FText Proclamation;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriumphProclaimed, int32, BossDexID, const TArray<FName>&, UnlockedStructureIds);

// Triumphs: combat and base, one loop. Felling a boss doesn't just
// give loot — it unlocks what the base can become. The triumphs are
// why you fight: every victory builds the home.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UTriumphSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Triumph")
    void RegisterTriumph(const FBossTriumph& Triumph);

    // The boss falls (never killed — subdued, purified, or driven off):
    // its triumph proclaims, and the base unlocks.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Triumph")
    void ReportBossDefeated(int32 BossDexID);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Triumph")
    bool IsStructureUnlocked(FName StructureId) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Triumph")
    TArray<FName> GetUnlockedStructures() const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Triumph")
    FOnTriumphProclaimed OnTriumphProclaimed;

private:
    UPROPERTY()
    TMap<int32, FBossTriumph> Triumphs;

    UPROPERTY()
    TSet<FName> UnlockedStructures;
};
