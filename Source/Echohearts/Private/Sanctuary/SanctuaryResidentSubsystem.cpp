// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Sanctuary/SanctuaryResidentSubsystem.h"

void USanctuaryResidentSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    RegisterFoundingResidents();
}

void USanctuaryResidentSubsystem::RegisterFoundingResidents()
{
    // The six founding roles. Names are titles, not people — the people who
    // fill them arrive through play, and the tamer learns their names.
    auto AddFounder = [this](const TCHAR* Id, const TCHAR* Name, ESanctuaryRole Role,
        EResidentArrivalKind Arrival, TArray<FString> Services, float Defense, const TCHAR* Line)
    {
        FSanctuaryResidentDef Def;
        Def.ResidentId = Id;
        Def.DisplayName = Name;
        Def.Role = Role;
        Def.Arrival = Arrival;
        Def.Services = MoveTemp(Services);
        Def.DefenseRating = Defense;
        Def.WelcomeLine = Line;
        RegisterResidentDef(Def);
    };

    AddFounder(TEXT("FounderMender"), TEXT("Mender"), ESanctuaryRole::Mender,
        EResidentArrivalKind::FirstGrowthRite,
        { TEXT("Heal Companion"), TEXT("Ease Link Strain") }, 12.0f,
        TEXT("I saw what the rite did for them. Let me do the same for you."));
    AddFounder(TEXT("FounderArtificer"), TEXT("Artificer"), ESanctuaryRole::Artificer,
        EResidentArrivalKind::FirstShedResources,
        { TEXT("Craft from Shed Resources"), TEXT("Forge Concord Catalyst") }, 10.0f,
        TEXT("Nothing given willingly should go to waste. Bring me what they shed."));
    AddFounder(TEXT("FounderChronicler"), TEXT("Chronicler"), ESanctuaryRole::Chronicler,
        EResidentArrivalKind::DexMilestone,
        { TEXT("Consult D.A.H.L.I.A."), TEXT("Read Lineage Record") }, 6.0f,
        TEXT("Every identity ever Bound is written down. Ask, and I will read."));
    AddFounder(TEXT("FounderBondkeeper"), TEXT("Bondkeeper"), ESanctuaryRole::Bondkeeper,
        EResidentArrivalKind::KindlingEighty,
        { TEXT("Kindling Guidance"), TEXT("Care Remedy") }, 8.0f,
        TEXT("Eighty percent is not a number. It is a promise kept daily."));
    AddFounder(TEXT("FounderPurifier"), TEXT("Purifier"), ESanctuaryRole::Purifier,
        EResidentArrivalKind::FirstPurification,
        { TEXT("Aid Purification"), TEXT("Cleanse Blight Taint") }, 14.0f,
        TEXT("I have walked the dark back before. Take my hand; we go together."));
    AddFounder(TEXT("FounderScout"), TEXT("Scout"), ESanctuaryRole::Scout,
        EResidentArrivalKind::RegionRestoredHalf,
        { TEXT("Region Intel"), TEXT("Mark Corrupted Territory") }, 11.0f,
        TEXT("Half a region healed means half its roads are safe. I know which half."));
}

void USanctuaryResidentSubsystem::RegisterResidentDef(const FSanctuaryResidentDef& Def)
{
    if (Def.ResidentId.IsNone())
    {
        return;
    }
    ResidentDefs.Add(Def.ResidentId, Def);
    if (!ResidentStates.Contains(Def.ResidentId))
    {
        FSanctuaryResidentState State;
        State.ResidentId = Def.ResidentId;
        ResidentStates.Add(Def.ResidentId, State);
    }
}

TArray<FName> USanctuaryResidentSubsystem::NotifyMilestone(EResidentArrivalKind Kind)
{
    TArray<FName> NewArrivals;
    for (const TPair<FName, FSanctuaryResidentDef>& Pair : ResidentDefs)
    {
        FSanctuaryResidentState* State = ResidentStates.Find(Pair.Key);
        if (!State || State->bMovedIn || Pair.Value.Arrival != Kind)
        {
            continue;
        }
        // The Sanctuary has proven itself; the resident comes home.
        State->bMovedIn = true;
        NewArrivals.Add(Pair.Key);
    }
    return NewArrivals;
}

bool USanctuaryResidentSubsystem::IsMovedIn(FName ResidentId) const
{
    const FSanctuaryResidentState* State = ResidentStates.Find(ResidentId);
    return State && State->bMovedIn;
}

TArray<FName> USanctuaryResidentSubsystem::GetMovedInResidents() const
{
    TArray<FName> Result;
    for (const TPair<FName, FSanctuaryResidentState>& Pair : ResidentStates)
    {
        if (Pair.Value.bMovedIn)
        {
            Result.Add(Pair.Key);
        }
    }
    return Result;
}

bool USanctuaryResidentSubsystem::TryGetResidentDef(FName ResidentId, FSanctuaryResidentDef& OutDef) const
{
    const FSanctuaryResidentDef* Def = ResidentDefs.Find(ResidentId);
    if (!Def)
    {
        return false;
    }
    OutDef = *Def;
    return true;
}

FString USanctuaryResidentSubsystem::RequestService(FName ResidentId, int32 ServiceIndex)
{
    FSanctuaryResidentState* State = ResidentStates.Find(ResidentId);
    const FSanctuaryResidentDef* Def = ResidentDefs.Find(ResidentId);
    if (!State || !Def || !State->bMovedIn)
    {
        return TEXT("No such resident has moved in.");
    }
    if (!Def->Services.IsValidIndex(ServiceIndex))
    {
        return TEXT("That service is not offered.");
    }
    State->ServicesUsed++;
    // Service effects (healing amounts, crafting results, intel payloads)
    // resolve in the gameplay layer; the subsystem records the exchange.
    return FString::Printf(TEXT("%s provides: %s"), *Def->DisplayName, *Def->Services[ServiceIndex]);
}

float USanctuaryResidentSubsystem::GetSanctuaryDefense(float RestorationPercent) const
{
    // Base ratings summed, then a restoration bonus: a Sanctuary that has
    // healed more of the world is defended by people with more to protect.
    float Total = 0.0f;
    for (const TPair<FName, FSanctuaryResidentState>& Pair : ResidentStates)
    {
        if (!Pair.Value.bMovedIn)
        {
            continue;
        }
        const FSanctuaryResidentDef* Def = ResidentDefs.Find(Pair.Key);
        if (Def)
        {
            Total += Def->DefenseRating;
        }
    }
    const float Clamped = FMath::Clamp(RestorationPercent, 0.0f, 100.0f);
    return Total * (1.0f + Clamped / 100.0f);
}

int32 USanctuaryResidentSubsystem::GetResidentCount() const
{
    return GetMovedInResidents().Num();
}
