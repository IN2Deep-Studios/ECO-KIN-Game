// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Corruption/CorruptionSubsystem.h"
#include "Companion/EcoKinCompanion.h"
#include "Companion/EcoKinAIController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UCorruptionSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    CorruptionMap.Empty();
    ExposedSet.Empty();
}

void UCorruptionSubsystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (ExposedSet.Num() == 0)
    {
        return;
    }
    // Standing in wounded ground corrupts, slowly and relentlessly.
    for (const TPair<int32, ECorruptionSource>& Pair : ExposedSet)
    {
        AccumulateCorruption(Pair.Key, ExposureRate * DeltaTime, Pair.Value);
    }
}

TStatId UCorruptionSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UCorruptionSubsystem, STATGROUP_Tickables);
}

void UCorruptionSubsystem::RegisterCorruptionForm(const FCorruptionFormDef& FormDef)
{
    if (FormDef.BaseDexID > 0)
    {
        FormDefs.Add(FormDef.BaseDexID, FormDef);
    }
}

void UCorruptionSubsystem::SetZoneExposure(int32 EcoKinID, bool bExposed, ECorruptionSource Source)
{
    if (EcoKinID <= 0)
    {
        return;
    }
    if (bExposed)
    {
        ExposedSet.Add(EcoKinID, Source);
    }
    else
    {
        ExposedSet.Remove(EcoKinID);
    }
}

void UCorruptionSubsystem::AccumulateCorruption(int32 EcoKinID, float Amount, ECorruptionSource Source)
{
    if (EcoKinID <= 0 || Amount <= 0.0f)
    {
        return;
    }
    FCorruptionState& State = CorruptionMap.FindOrAdd(EcoKinID);
    if (State.DominantSource == ECorruptionSource::None)
    {
        State.DominantSource = Source;
    }

    const ECorruptionStage Before = State.GetStage();
    State.Level = FMath::Clamp(State.Level + Amount, 0.0f, 100.0f);
    const ECorruptionStage After = State.GetStage();

    AEcoKinCompanion* Companion = FindCompanion(EcoKinID);

    // Crossing into Corrupted: the form manifests.
    if (!State.bFormActive && State.Level >= FormThreshold && After != ECorruptionStage::Fractured)
    {
        ApplyCorruptionForm(EcoKinID, Companion);
    }

    // At 100 the Echoheart fractures. This is a CONDITION, not a death and
    // not a form — the creature is lost to Shadow until it is brought back.
    if (After == ECorruptionStage::Fractured && Before != ECorruptionStage::Fractured)
    {
        if (State.bFormActive)
        {
            RevertCorruptionForm(EcoKinID, Companion);
        }
        if (Companion)
        {
            // A fractured one cannot be reasoned with — only survived,
            // avoided, or one day purified.
            if (AEcoKinAIController* AI = Cast<AEcoKinAIController>(Companion->GetController()))
            {
                AI->RequestStance(ECompanionStance::Engage, 0.0f, 100.0f);
            }
        }
    }
}

void UCorruptionSubsystem::Purify(int32 EcoKinID, float Amount, const FString& Method)
{
    if (EcoKinID <= 0 || Amount <= 0.0f)
    {
        return;
    }
    FCorruptionState* State = CorruptionMap.Find(EcoKinID);
    if (!State || State->Level <= 0.0f)
    {
        return;
    }

    // Purification never touches the Fractured — that road requires a rite,
    // not a routine. (Major purification rites are story-gated.)
    if (State->GetStage() == ECorruptionStage::Fractured)
    {
        return;
    }

    State->Level = FMath::Clamp(State->Level - Amount, 0.0f, 100.0f);

    // The form falls away when corruption drops back below the revert line.
    // What was there is restored exactly — healing leaves no scar on the stats.
    if (State->bFormActive && State->Level < RevertThreshold)
    {
        RevertCorruptionForm(EcoKinID, FindCompanion(EcoKinID));
    }

    if (State->Level <= 0.0f)
    {
        State->DominantSource = ECorruptionSource::None;
    }
}

float UCorruptionSubsystem::GetCorruptionLevel(int32 EcoKinID) const
{
    const FCorruptionState* State = CorruptionMap.Find(EcoKinID);
    return State ? State->Level : 0.0f;
}

ECorruptionStage UCorruptionSubsystem::GetCorruptionStage(int32 EcoKinID) const
{
    const FCorruptionState* State = CorruptionMap.Find(EcoKinID);
    return State ? State->GetStage() : ECorruptionStage::Clean;
}

bool UCorruptionSubsystem::IsCorruptionFormActive(int32 EcoKinID) const
{
    const FCorruptionState* State = CorruptionMap.Find(EcoKinID);
    return State && State->bFormActive;
}

bool UCorruptionSubsystem::TryGetCorruptionForm(int32 BaseDexID, FCorruptionFormDef& OutDef) const
{
    const FCorruptionFormDef* Def = FormDefs.Find(BaseDexID);
    if (!Def)
    {
        return false;
    }
    OutDef = *Def;
    return true;
}

void UCorruptionSubsystem::ApplyCorruptionForm(int32 EcoKinID, AEcoKinCompanion* Companion)
{
    FCorruptionState* State = CorruptionMap.Find(EcoKinID);
    if (!State || State->bFormActive)
    {
        return;
    }
    State->bFormActive = true;

    if (Companion)
    {
        // Store the originals — purification restores exactly.
        State->OriginalVibrance = Companion->Attributes.Vibrance;
        State->OriginalDensity = Companion->Attributes.Density;
        State->OriginalHarmony = Companion->Attributes.Harmony;
        State->OriginalPurity = Companion->Attributes.Purity;

        // Corruption redistributes: it borrows from Harmony and Purity to
        // feed Vibrance and Density. The budget never grows — the creature
        // is not stronger, it is more desperate.
        const float Borrowed = (Companion->Attributes.Harmony + Companion->Attributes.Purity) * 0.25f;
        Companion->Attributes.Harmony = FMath::Max(5.0f, Companion->Attributes.Harmony - Borrowed * 0.6f);
        Companion->Attributes.Purity = FMath::Max(5.0f, Companion->Attributes.Purity - Borrowed * 0.4f);
        Companion->Attributes.Vibrance = FMath::Min(100.0f, Companion->Attributes.Vibrance + Borrowed * 0.55f);
        Companion->Attributes.Density = FMath::Min(100.0f, Companion->Attributes.Density + Borrowed * 0.45f);
    }
}

void UCorruptionSubsystem::RevertCorruptionForm(int32 EcoKinID, AEcoKinCompanion* Companion)
{
    FCorruptionState* State = CorruptionMap.Find(EcoKinID);
    if (!State || !State->bFormActive)
    {
        return;
    }
    State->bFormActive = false;

    if (Companion)
    {
        Companion->Attributes.Vibrance = State->OriginalVibrance;
        Companion->Attributes.Density = State->OriginalDensity;
        Companion->Attributes.Harmony = State->OriginalHarmony;
        Companion->Attributes.Purity = State->OriginalPurity;
    }
}

AEcoKinCompanion* UCorruptionSubsystem::FindCompanion(int32 EcoKinID) const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }
    for (TActorIterator<AEcoKinCompanion> It(World); It; ++It)
    {
        if (It->EcoKinID == EcoKinID)
        {
            return *It;
        }
    }
    return nullptr;
}
