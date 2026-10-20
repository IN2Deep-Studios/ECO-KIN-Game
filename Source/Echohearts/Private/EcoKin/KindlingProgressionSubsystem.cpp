// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "EcoKin/KindlingProgressionSubsystem.h"

float UKindlingProgressionSubsystem::RecordTrustEvent(int32 EcoKinID, EKindlingTrustEvent Event)
{
    FKindlingState& State = KindlingByEcoKinID.FindOrAdd(EcoKinID);
    State.TrustPercent += TrustDeltaFor(Event);
    State.Clamp();
    return State.TrustPercent;
}

bool UKindlingProgressionSubsystem::TryGetKindling(int32 EcoKinID, FKindlingState& OutKindling) const
{
    const FKindlingState* Found = KindlingByEcoKinID.Find(EcoKinID);
    if (!Found)
    {
        return false;
    }
    OutKindling = *Found;
    return true;
}

void UKindlingProgressionSubsystem::SetKindling(int32 EcoKinID, const FKindlingState& Kindling)
{
    FKindlingState Clamped = Kindling;
    Clamped.Clamp();
    KindlingByEcoKinID.Add(EcoKinID, Clamped);
}

float UKindlingProgressionSubsystem::TrustDeltaFor(EKindlingTrustEvent Event) const
{
    // Trust is earned through the relationship. There is no XP, no level, and
    // no grind multiplier anywhere in this table — only what happened.
    switch (Event)
    {
    case EKindlingTrustEvent::CareGiven:            return 2.0f;
    case EKindlingTrustEvent::BattleFoughtTogether: return 3.0f;
    case EKindlingTrustEvent::RestorationDeed:      return 5.0f;
    case EKindlingTrustEvent::TimeBonded:           return 1.0f;
    case EKindlingTrustEvent::Neglect:              return -3.0f;
    case EKindlingTrustEvent::HarmDone:             return -10.0f;
    case EKindlingTrustEvent::ForcedAction:         return -8.0f;
    default:                                        return 0.0f;
    }
}
