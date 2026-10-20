// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "WorkAptitudeTypes.generated.h"

// What an EcoKin is naturally good at — the work their body and essence
// were shaped for. Aptitudes are innate to the species (ecology, not
// training), ranked in tiers 1-5, and deepen through Growth Rites as
// the EcoKin matures. This is the method of work suitability, rebuilt
// with our names, our essences, and our structures.
UENUM(BlueprintType)
enum class EWorkAptitude : uint8
{
    None         UMETA(DisplayName = "None"),
    // Pyre: heat for the forge and the kitchen. (Kindling is trust in
    // our game — never fire. This is Emberheart.)
    Emberheart   UMETA(DisplayName = "Emberheart"),
    // Torrent: directing water — irrigation, purification cycling.
    Tidecalling  UMETA(DisplayName = "Tidecalling"),
    // Aero: wind and wings — spotting, swiftness, messages.
    Galecalling  UMETA(DisplayName = "Galecalling"),
    // Terra: reading stone — crystal and mineral harvest.
    Stonewise    UMETA(DisplayName = "Stonewise"),
    // Flora: sustainable lumber — separating wood without killing trees.
    Timbercraft  UMETA(DisplayName = "Timbercraft"),
    // Flora: planting and herbcraft — seeds, remedies, gardens.
    Florawhisper UMETA(DisplayName = "Florawhisper"),
    // Glaze: frost — cooling, preservation, keeping food true.
    Frostbound   UMETA(DisplayName = "Frostbound"),
    // Voltic: living current — power for the works.
    Voltspark    UMETA(DisplayName = "Voltspark"),
    // Any: hauling and logistics — clean floors, full vaults.
    Swiftcarry   UMETA(DisplayName = "Swiftcarry"),
    // Aura: morale and music — harmony made by hand.
    Dreamweave   UMETA(DisplayName = "Dreamweave"),
    // Any: guarding — walking the walls, watching the dark.
    Beastguard   UMETA(DisplayName = "Beastguard"),
    // Any: passive shedding — the meadow's gentle tithe. Shed and
    // gifted resources only; never anything taken, never spheres.
    Meadowgift   UMETA(DisplayName = "Meadowgift")
};

// One species' innate work profile: aptitude -> tier (1-5).
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FSpeciesWorkProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Work")
    int32 DexID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Work")
    TMap<EWorkAptitude, int32> AptitudeTiers;

    int32 GetTier(EWorkAptitude Aptitude) const
    {
        const int32* Tier = AptitudeTiers.Find(Aptitude);
        return Tier ? FMath::Clamp(*Tier, 1, 5) : 0;
    }

    bool HasAptitude(EWorkAptitude Aptitude) const { return AptitudeTiers.Contains(Aptitude); }
};
