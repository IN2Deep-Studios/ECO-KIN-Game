// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldTypes.generated.h"

// Forecast-driven weather. Fronts move across regions on a 24-hour cycle the
// A.E.G.I.S. device can forecast; severe cells (tornado, hurricane) have
// lifecycles the player can read and avoid. Weather touches the Anima-Link:
// storms raise strain costs, clear skies ease recovery.
UENUM(BlueprintType)
enum class EWeatherState : uint8
{
    Clear      UMETA(DisplayName = "Clear"),
    Rain       UMETA(DisplayName = "Rain"),
    Snow       UMETA(DisplayName = "Snow"),
    Storm      UMETA(DisplayName = "Storm"),
    Tornado    UMETA(DisplayName = "Tornado"),
    Hurricane  UMETA(DisplayName = "Hurricane")
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FRegionInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|World")
    FName RegionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|World")
    FText DisplayName;

    // 0-100. Rises as restoration deeds complete; at 100 the region is healed.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|World")
    float RestorationPercent = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|World")
    EWeatherState CurrentWeather = EWeatherState::Clear;

    // The next 24 hours, for the A.E.G.I.S. forecast UI.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|World")
    TArray<EWeatherState> Forecast24h;
};
