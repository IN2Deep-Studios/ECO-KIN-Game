// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "AnimaLinkTypes.generated.h"

// Locked canon: the Anima-Link is BIDIRECTIONAL. Eco-Kin strain/damage feeds back
// as stamina/health consequences on the tamer. Transformations carry Link Strain costs.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FAnimaLinkState
{
    GENERATED_BODY()

    // Accumulated strain on the link. 100% risks link fracture (a Shadow-fracture
    // condition trigger), never a silent power-up.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AnimaLink", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float LinkStrain = 0.0f;

    // Fraction of bonded Eco-Kin strain that feeds back onto tamer stamina.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AnimaLink", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float TamerStaminaFeedback = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AnimaLink")
    bool bLinkActive = false;

    void ApplyStrain(float Amount)
    {
        LinkStrain = FMath::Clamp(LinkStrain + Amount, 0.0f, 100.0f);
    }

    void ReleaseStrain(float Amount)
    {
        LinkStrain = FMath::Clamp(LinkStrain - Amount, 0.0f, 100.0f);
    }

    bool IsFractureRisk() const { return LinkStrain >= 100.0f; }
};
