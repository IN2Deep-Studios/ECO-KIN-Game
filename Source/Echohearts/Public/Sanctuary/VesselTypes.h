// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "BaseBuilding/BaseStructureTypes.h"
#include "VesselTypes.generated.h"

// Where the Elderwing is.
UENUM(BlueprintType)
enum class EVesselState : uint8
{
    DockedSkyhold UMETA(DisplayName = "Docked at Skyhold"),
    DockedBase    UMETA(DisplayName = "Docked at Sanctuary"),
    Aloft        UMETA(DisplayName = "Aloft")
};

// The Elderwing: a living elder-root skyship, the Tamer's flying
// sanctuary. Extra bound companions roost here — never NPCs (the
// Tamer's rule) — and the battle roster is chosen here. She flies;
// she never teleports. Travel is flight or ride, always.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FVesselData
{
    GENERATED_BODY()

    // Her name, chosen 2026-10-19. The Tamer may rename her, but the
    // ancestors will always call her Elderwing.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Vessel")
    FText VesselName = FText::FromString(TEXT("Elderwing"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Vessel")
    int32 Tier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Vessel")
    EVesselState State = EVesselState::DockedBase;

    // Roost berths: how many companions can roost aboard.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Vessel")
    int32 RoostCapacity = 12;

    // Sky-garden terraces: total vs cleansed of blight-ash.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Vessel")
    int32 GardenTerracesTotal = 6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Vessel")
    int32 GardenTerracesCleansed = 1;
};
