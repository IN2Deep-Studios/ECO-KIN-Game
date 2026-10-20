#pragma once

#include "CoreMinimal.h"
#include "EcoKinCoreAttributes.generated.h"

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FEcoKinCoreAttributes
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Attributes", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Vibrance = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Attributes", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Density = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Attributes", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Harmony = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Attributes", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Purity = 50.0f;

    void Clamp()
    {
        Vibrance = FMath::Clamp(Vibrance, 0.0f, 100.0f);
        Density = FMath::Clamp(Density, 0.0f, 100.0f);
        Harmony = FMath::Clamp(Harmony, 0.0f, 100.0f);
        Purity = FMath::Clamp(Purity, 0.0f, 100.0f);
    }
};
