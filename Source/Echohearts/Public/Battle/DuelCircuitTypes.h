// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "DuelCircuitTypes.generated.h"

// Circuit ranks. Musical, not military — the Circuit is sport.
UENUM(BlueprintType)
enum class ECircuitRank : uint8
{
    Hum      UMETA(DisplayName = "Hum"),
    Chord    UMETA(DisplayName = "Chord"),
    Chorus   UMETA(DisplayName = "Chorus"),
    Symphony UMETA(DisplayName = "Symphony")
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FRivalTamer
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Battle")
    FName RivalId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Battle")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Battle")
    FText Style;

    // Partner Dex IDs the rival fields.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Battle")
    TArray<int32> PartnerEcoKinIds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Battle")
    ECircuitRank Rank = ECircuitRank::Hum;
};
