// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "TamerTypes.generated.h"

// The tamer's own vitals. Canon: the Anima-Link is bidirectional — when a
// bonded Eco-Kin takes strain or damage, it feeds back here as stamina and
// health consequences on the tamer. There is no one-sided damage model.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FTamerVitals
{
    GENERATED_BODY()

    // 0-100. Spent on sprinting, dodging, and commanding; drained by
    // Anima-Link feedback when the bonded Eco-Kin suffers.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Tamer")
    float Stamina = 100.0f;

    // 0-100. Only damaged through heavy link feedback or world hazards —
    // enemies target the Eco-Kin in combat, never the tamer directly.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Tamer")
    float Health = 100.0f;

    void Clamp()
    {
        Stamina = FMath::Clamp(Stamina, 0.0f, 100.0f);
        Health = FMath::Clamp(Health, 0.0f, 100.0f);
    }

    bool IsExhausted() const { return Stamina <= 0.5f; }
    bool IsDown() const { return Health <= 0.5f; }
};

// A.E.G.I.S. device state. The device is the tamer's instrument: scanning,
// Dex lookup, resonance tuning — never a weapon.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FAegisDeviceState
{
    GENERATED_BODY()

    // 0-100. Scans and resonance tuning draw charge; recharges at rest and at
    // attuned nexuses.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AEGIS")
    float Charge = 100.0f;

    // True while the device is actively scanning the environment.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AEGIS")
    bool bScanning = false;

    void Clamp() { Charge = FMath::Clamp(Charge, 0.0f, 100.0f); }
};
