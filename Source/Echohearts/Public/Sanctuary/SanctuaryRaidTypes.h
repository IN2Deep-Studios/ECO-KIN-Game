// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SanctuaryRaidTypes.generated.h"

// One raiding wave.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FRaidWave
{
    GENERATED_BODY()

    // Dex IDs of the raiders.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    TArray<int32> RaiderSpeciesIds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    int32 CountPerSpecies = 2;

    // Seconds after the raid starts when this wave arrives.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    float ArriveAtSeconds = 15.0f;
};

UENUM(BlueprintType)
enum class ERaidState : uint8
{
    Quiet    UMETA(DisplayName = "Quiet"),
    Warned   UMETA(DisplayName = "Warned"),
    Underway UMETA(DisplayName = "Underway"),
    Repelled UMETA(DisplayName = "Repelled"),
    Breached UMETA(DisplayName = "Breached")
};

// A sanctuary raid: the societies (or the blight) probe your home.
// Warning first, waves second, aftermath third. Structures soften it;
// residents and the tamer decide it. Damage, never destruction.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FSanctuaryRaidDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    FName RaidId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    FText Title;

    // Seconds of warning before the first wave (the Scout's sky-watch).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    float WarningSeconds = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    TArray<FRaidWave> Waves;

    // Defense rating at or above which the raid breaks early (repelled).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    float RepelDefenseThreshold = 40.0f;

    // Damage dealt to a placed structure per unresolved wave, every
    // DamageIntervalSeconds. Go hard or lose: ignored waves break things.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    float StructureDamagePerInterval = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    float DamageIntervalSeconds = 10.0f;

    // Structure ID of the base anchor (the Heartroot Spire). If it falls
    // mid-raid, the base is overrun: the raid ends breached immediately.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Sanctuary")
    FName AnchorStructureId = FName("heartroot_spire");
};
