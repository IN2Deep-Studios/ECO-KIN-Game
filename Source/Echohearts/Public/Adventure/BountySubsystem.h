// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BountySubsystem.generated.h"

UENUM(BlueprintType)
enum class EBountyState : uint8
{
    Posted    UMETA(DisplayName = "Posted"),
    Claimed   UMETA(DisplayName = "Claimed"),
    Fulfilled UMETA(DisplayName = "Fulfilled")
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FBountyPosting
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Adventure")
    FName BountyId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Adventure")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Adventure")
    FText Description;

    // Reward is resources/recognition — never a price on a life.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Adventure")
    FText Reward;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBountyChanged, FName, BountyId, EBountyState, State);

// The Bounty Board: posted at the Sanctuary — society targets,
// rescue missions, blight nests. Chosen freely, fulfilled honorably.
// Rewards are resources and recognition; never a price on a life.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UBountySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    void PostBounty(const FBountyPosting& Posting);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    bool ClaimBounty(FName BountyId);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Adventure")
    bool ReportBountyFulfilled(FName BountyId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Adventure")
    TArray<FBountyPosting> GetPostedBounties() const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Adventure")
    FOnBountyChanged OnBountyChanged;

private:
    UPROPERTY()
    TMap<FName, FBountyPosting> Postings;

    UPROPERTY()
    TMap<FName, EBountyState> States;
};
