// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "DiscoveryTypes.generated.h"

// What a discovery reward can grant. Resources are shed/gifted bundles and
// star-tier materials; cosmetics are looks; secret items are unique finds.
// Nothing here is power you can't earn elsewhere — discovery is for the
// curious, never a toll road.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FDiscoveryReward
{
    GENERATED_BODY()

    // Resource IDs + unit counts granted (shed/gifted only, per the
    // no-sacrifice rule).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    TMap<FName, int32> Resources;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    TArray<FName> CosmeticIds;

    // Secret item granted, if any (also unlocked via UDiscoverySubsystem).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FName SecretItemId = NAME_None;
};

// One clue in a treasure hunt. Clues solve in order: a riddle, a landmark,
// or an A.E.G.I.S. scan anomaly — never a waypoint. The hunt is the fun.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FHuntClue
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText Riddle;

    // Region tag where this clue can be found (map shows the region, never
    // the spot).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FName RegionTag = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    bool bSolved = false;
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FTreasureHuntDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FName HuntId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    TArray<FHuntClue> Clues;

    // Granted once, when the final clue solves. One-per-tamer.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FDiscoveryReward CacheReward;
};

// One quiz question. The Chronicler always explains the right answer —
// wrong answers teach, never punish.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FQuizQuestion
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText Question;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    TArray<FText> Options;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    int32 CorrectOptionIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText Explanation;
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FQuizTopicDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FName TopicId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText WhatItTeaches;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    TArray<FQuizQuestion> Questions;

    // Loot for completing (not perfecting) the quiz.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FDiscoveryReward CompletionReward;

    // Bonus cache for a perfect score.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FDiscoveryReward PerfectReward;
};

// A secret item: unique, hinted by riddle, never marked. One per tamer.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FSecretItemDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FName ItemId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText Name;

    // The riddle-hint. The game never spoils more than this.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText Hint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Discovery")
    FText Story;
};
