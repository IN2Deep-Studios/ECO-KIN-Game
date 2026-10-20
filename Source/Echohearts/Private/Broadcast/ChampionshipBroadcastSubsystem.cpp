// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Broadcast/ChampionshipBroadcastSubsystem.h"
#include "Broadcast/ChampionshipTVActor.h"

void UChampionshipBroadcastSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    RegisteredTVs.Reset();
}

void UChampionshipBroadcastSubsystem::Deinitialize()
{
    RegisteredTVs.Reset();
    Super::Deinitialize();
}

void UChampionshipBroadcastSubsystem::RegisterTV(AChampionshipTV* TV)
{
    if (TV == nullptr)
    {
        return;
    }
    if (!RegisteredTVs.Contains(TV))
    {
        RegisteredTVs.Add(TV);
    }
}

void UChampionshipBroadcastSubsystem::UnregisterTV(AChampionshipTV* TV)
{
    if (TV == nullptr)
    {
        return;
    }
    RegisteredTVs.Remove(TV);
}

void UChampionshipBroadcastSubsystem::SetLiveReel(int32 ReelIndex)
{
    for (TWeakObjectPtr<AChampionshipTV> WeakTV : RegisteredTVs)
    {
        AChampionshipTV* TV = WeakTV.Get();
        if (TV != nullptr)
        {
            TV->PlayReel(ReelIndex);
        }
    }
    // Prune any TVs that were destroyed without unregistering.
    RegisteredTVs.RemoveAll([](const TWeakObjectPtr<AChampionshipTV>& WeakTV)
    {
        return !WeakTV.IsValid();
    });
}

void UChampionshipBroadcastSubsystem::StopAllBroadcasts()
{
    for (TWeakObjectPtr<AChampionshipTV> WeakTV : RegisteredTVs)
    {
        AChampionshipTV* TV = WeakTV.Get();
        if (TV != nullptr)
        {
            TV->StopBroadcast();
        }
    }
}

int32 UChampionshipBroadcastSubsystem::GetRegisteredTVCount() const
{
    int32 Count = 0;
    for (const TWeakObjectPtr<AChampionshipTV>& WeakTV : RegisteredTVs)
    {
        if (WeakTV.IsValid())
        {
            ++Count;
        }
    }
    return Count;
}
