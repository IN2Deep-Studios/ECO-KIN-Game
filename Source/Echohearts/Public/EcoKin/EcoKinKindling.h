// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "EcoKinKindling.generated.h"

// Locked canon: progression runs on Growth Rites, not level-reset evolutions.
// Kindling (trust %) replaces the XP-grind feel. A Growth Rite is gated by
// Biological Maturity, Kindling, Ecology accomplishments, and Catalyst items.
// Echoprints remain permanent through every rite.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FKindlingState
{
    GENERATED_BODY()

    // Trust percentage. Replaces the XP-grind feel.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Kindling", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float TrustPercent = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Kindling")
    bool bBiologicallyMature = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Kindling")
    int32 EcologyAccomplishments = 0;

    // Catalyst / Evolution Stone item IDs (e.g. Bloomstone, Torrentstone).
    // Shadestone is Shade attunement, never corruption.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Kindling")
    TArray<FName> CatalystItemIDs;

    // Canonical gate: 80%+ Bond is the standard Growth Rite threshold.
    bool CanPerformGrowthRite(int32 RequiredAccomplishments) const
    {
        return TrustPercent >= 80.0f
            && bBiologicallyMature
            && EcologyAccomplishments >= RequiredAccomplishments
            && CatalystItemIDs.Num() > 0;
    }

    void Clamp()
    {
        TrustPercent = FMath::Clamp(TrustPercent, 0.0f, 100.0f);
    }
};
