// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Encounter/EncounterSubsystem.h"
#include "Companion/EcoKinCompanion.h"
#include "Encounter/WildBehaviorComponent.h"

void UEncounterSubsystem::Deinitialize()
{
    ActiveEncounters.Empty();
    Super::Deinitialize();
}

void UEncounterSubsystem::BeginEncounter(AEcoKinCompanion* WildEcoKin, AActor* Tamer)
{
    if (!IsValid(WildEcoKin) || ActiveEncounters.Contains(WildEcoKin))
    {
        return;
    }

    FActiveEncounter Record;
    Record.Tamer = Tamer;
    Record.Phase = EEncounterPhase::Engaged;
    Record.StartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    Record.EcoKinID = WildEcoKin->EcoKinID;
    if (UWildBehaviorComponent* Behavior = WildEcoKin->FindComponentByClass<UWildBehaviorComponent>())
    {
        Record.State = Behavior->EncounterState;
    }
    ActiveEncounters.Add(WildEcoKin, Record);

    OnEncounterStarted.Broadcast(WildEcoKin, Tamer);
}

void UEncounterSubsystem::EndEncounter(AEcoKinCompanion* WildEcoKin, EEncounterResolution Resolution)
{
    FActiveEncounter Record;
    if (!ActiveEncounters.RemoveAndCopyValue(WildEcoKin, Record))
    {
        return;
    }

    FEncounterRecord History;
    History.EcoKinID = Record.EcoKinID;
    History.State = Record.State;
    History.Resolution = Resolution;
    History.DurationSeconds = (GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f) - Record.StartTime;
    EncounterHistory.Add(History);

    OnEncounterEnded.Broadcast(History.EcoKinID, History.State, Resolution);
}

void UEncounterSubsystem::OfferBinding(AEcoKinCompanion* WildEcoKin, AActor* Tamer)
{
    if (!IsValid(WildEcoKin))
    {
        return;
    }
    if (FActiveEncounter* Record = ActiveEncounters.Find(WildEcoKin))
    {
        Record->Phase = EEncounterPhase::BindingOffered;
    }
    OnBindingOffered.Broadcast(WildEcoKin, Tamer);
}

bool UEncounterSubsystem::IsInEncounter(AEcoKinCompanion* WildEcoKin) const
{
    return ActiveEncounters.Contains(WildEcoKin);
}

int32 UEncounterSubsystem::GetActiveEncounterCount() const
{
    return ActiveEncounters.Num();
}
