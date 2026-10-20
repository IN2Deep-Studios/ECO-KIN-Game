// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "CorruptionTypes.generated.h"

// Where corruption comes from. Every source is a wound in Rebearth —
// corruption is never random, never "evil for fun."
UENUM(BlueprintType)
enum class ECorruptionSource : uint8
{
    None               UMETA(DisplayName = "None"),
    BlightZone         UMETA(DisplayName = "Blight Zone"),        // environmental collapse
    DominationRemnant  UMETA(DisplayName = "Domination Remnant"), // ruins of forced domination
    TamperingSite      UMETA(DisplayName = "Tampering Site"),     // DNA over-tampering labs
    WeaponCache        UMETA(DisplayName = "Weapon Cache"),       // weaponization remnants
    Neglect            UMETA(DisplayName = "Neglect"),            // a bond left untended
    ForcedStrain       UMETA(DisplayName = "Forced Strain")       // commands that broke trust
};

// Corruption is a path, not a switch. Each stage changes what the EcoKin
// is — and what it can still be saved from.
UENUM(BlueprintType)
enum class ECorruptionStage : uint8
{
    Clean      UMETA(DisplayName = "Clean"),      // 0–24: untainted
    Tainted    UMETA(DisplayName = "Tainted"),    // 25–49: veins show, self still in control
    Corrupted  UMETA(DisplayName = "Corrupted"),  // 50–74: Corruption form manifests
    Severe     UMETA(DisplayName = "Severe"),     // 75–99: fracture risk climbs every hour
    Fractured  UMETA(DisplayName = "Fractured")   // 100: Echoheart fracture — Shadow condition
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FCorruptionState
{
    GENERATED_BODY()

    // 0–100. Never exceeds; never goes below 0.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Corruption")
    float Level = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Corruption")
    ECorruptionSource DominantSource = ECorruptionSource::None;

    // True while the Corruption form is worn. Forms attach to identity —
    // the Echoprint never changes, the expression does.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Corruption")
    bool bFormActive = false;

    // Original attributes, stored when the form first manifests so
    // purification restores exactly what was there.
    float OriginalVibrance = 0.0f;
    float OriginalDensity = 0.0f;
    float OriginalHarmony = 0.0f;
    float OriginalPurity = 0.0f;

    ECorruptionStage GetStage() const
    {
        if (Level >= 100.0f) return ECorruptionStage::Fractured;
        if (Level >= 75.0f)  return ECorruptionStage::Severe;
        if (Level >= 50.0f)  return ECorruptionStage::Corrupted;
        if (Level >= 25.0f)  return ECorruptionStage::Tainted;
        return ECorruptionStage::Clean;
    }
};

// One designed Corruption form: the dark mirror of a known EcoKin.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FCorruptionFormDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Corruption")
    int32 BaseDexID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Corruption")
    FString CorruptionName;

    // Where this corrupted one is encountered around the map.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Corruption")
    FString Territory;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Corruption")
    FString Description;
};
