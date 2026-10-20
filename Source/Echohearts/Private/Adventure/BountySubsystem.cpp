// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Adventure/BountySubsystem.h"

void UBountySubsystem::PostBounty(const FBountyPosting& Posting)
{
    if (Posting.BountyId.IsNone())
    {
        return;
    }
    Postings.Add(Posting.BountyId, Posting);
    States.Add(Posting.BountyId, EBountyState::Posted);
    OnBountyChanged.Broadcast(Posting.BountyId, EBountyState::Posted);
}

bool UBountySubsystem::ClaimBounty(FName BountyId)
{
    EBountyState* State = States.Find(BountyId);
    if (!State || *State != EBountyState::Posted)
    {
        return false;
    }
    *State = EBountyState::Claimed;
    OnBountyChanged.Broadcast(BountyId, *State);
    return true;
}

bool UBountySubsystem::ReportBountyFulfilled(FName BountyId)
{
    EBountyState* State = States.Find(BountyId);
    if (!State || *State != EBountyState::Claimed)
    {
        return false;
    }
    *State = EBountyState::Fulfilled;
    OnBountyChanged.Broadcast(BountyId, *State);
    return true;
}

TArray<FBountyPosting> UBountySubsystem::GetPostedBounties() const
{
    TArray<FBountyPosting> Out;
    for (const auto& Pair : Postings)
    {
        const EBountyState* State = States.Find(Pair.Key);
        if (State && *State == EBountyState::Posted)
        {
            Out.Add(Pair.Value);
        }
    }
    return Out;
}
