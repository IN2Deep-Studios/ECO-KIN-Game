// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Binding/BindingCeremonySubsystem.h"
#include "Binding/BindingTypes.h"
#include "Encounter/EncounterSubsystem.h"
#include "Encounter/EncounterTypes.h"
#include "Encounter/WildBehaviorComponent.h"
#include "Companion/EcoKinCompanion.h"
#include "AnimaLink/AnimaLinkComponent.h"
#include "EcoKin/KindlingProgressionSubsystem.h"
#include "EcoKin/EcoKinKindling.h"
#include "Corruption/CorruptionSubsystem.h"
#include "Game/EchoheartsGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UBindingCeremonySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (UEncounterSubsystem* Encounters = GetEncounterSubsystem())
    {
        // The door the encounter system built: offers arrive here.
        Encounters->OnBindingOffered.AddDynamic(this, &UBindingCeremonySubsystem::HandleBindingOffered);
    }
}

void UBindingCeremonySubsystem::Deinitialize()
{
    if (UEncounterSubsystem* Encounters = GetEncounterSubsystem())
    {
        Encounters->OnBindingOffered.RemoveDynamic(this, &UBindingCeremonySubsystem::HandleBindingOffered);
    }
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(PulseTimerHandle);
        World->GetTimerManager().ClearTimer(PhaseTimerHandle);
    }
    Super::Deinitialize();
}

UEncounterSubsystem* UBindingCeremonySubsystem::GetEncounterSubsystem() const
{
    UWorld* World = GetWorld();
    return World ? World->GetSubsystem<UEncounterSubsystem>() : nullptr;
}

void UBindingCeremonySubsystem::HandleBindingOffered(AEcoKinCompanion* WildEcoKin, AActor* Tamer)
{
    if (!WildEcoKin || !Tamer || IsCeremonyActive())
    {
        // Nulls are ignored. One ceremony at a time: a second offer simply
        // waits, the way an unanswered offer is never a trap.
        return;
    }

    const int32 EcoKinID = WildEcoKin->EcoKinID;

    // Canon refusal: some walk their own path. Nature is the canonical entry.
    if (Config.NeverBindableIDs.Contains(EcoKinID))
    {
        FBindingRecord Record;
        Record.EcoKinID = EcoKinID;
        Record.Outcome = EBindingOutcome::RefusedByCanon;
        BindingHistory.Add(Record);
        OnCeremonyCompleted.Broadcast(EBindingOutcome::RefusedByCanon);
        return;
    }

    // Dignity timer: a recent decline is respected, not re-asked.
    if (const float* LastOffer = LastOfferTimeByEcoKinID.Find(EcoKinID))
    {
        const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
        if (Now - *LastOffer < Config.OfferCooldownSeconds)
        {
            return;
        }
    }

    // Ceremony never ambushes combat: if a duel is running, the offer waits.
    if (AEchoheartsGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AEchoheartsGameMode>() : nullptr)
    {
        if (GM->CurrentPhase == EGamePhase::Combat)
        {
            return;
        }
    }

    EEncounterState WildState = EEncounterState::Calm;
    if (UWildBehaviorComponent* Behavior = WildEcoKin->FindComponentByClass<UWildBehaviorComponent>())
    {
        WildState = Behavior->EncounterState;
    }

    BeginCeremony(WildEcoKin, Tamer, WildState);
}

void UBindingCeremonySubsystem::BeginCeremony(AEcoKinCompanion* WildEcoKin, AActor* Tamer, EEncounterState WildState)
{
    Active.WildEcoKin = WildEcoKin;
    Active.Tamer = Tamer;
    Active.WildState = WildState;
    Active.EcoKinID = WildEcoKin->EcoKinID;
    Active.PulseSyncs = 0;
    Active.PulseAttempts = 0;
    Active.LastBeatTime = -1.0f;
    Active.bPurifiedDuringCeremony = false;
    Active.bStrainHoldNotified = false;
    Active.CeremonyStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

    // The ceremony locks combat input: rites are never interrupted by a stray attack.
    if (AEchoheartsGameMode* GM = GetWorld()->GetAuthGameMode<AEchoheartsGameMode>())
    {
        GM->SetPhase(EGamePhase::Ceremony);
    }

    SetPhase(EBindingPhase::Approach);

    // The Approach beat: the tamer enters, the wild one is invited. Then the
    // real work begins.
    GetWorld()->GetTimerManager().SetTimer(
        PhaseTimerHandle, this, &UBindingCeremonySubsystem::OnApproachFinished,
        Config.ApproachDurationSeconds, false);
}

void UBindingCeremonySubsystem::SetPhase(EBindingPhase NewPhase)
{
    const EBindingPhase OldPhase = Active.Phase;
    Active.Phase = NewPhase;
    OnPhaseChanged.Broadcast(OldPhase, NewPhase);
}

void UBindingCeremonySubsystem::OnApproachFinished()
{
    if (!IsCeremonyActive() || Active.Phase != EBindingPhase::Approach)
    {
        return;
    }
    SetPhase(EBindingPhase::Attunement);

    // Fallback pulse until animation notifies drive NotifyPulseBeat.
    GetWorld()->GetTimerManager().SetTimer(
        PulseTimerHandle, this, &UBindingCeremonySubsystem::OnPulseTick,
        Config.PulseIntervalSeconds, true);

    // Attunement gives up gently: a wild one that never feels the rhythm
    // simply loses interest. Never forced.
    GetWorld()->GetTimerManager().SetTimer(
        PhaseTimerHandle, this, &UBindingCeremonySubsystem::OnAttunementTimeout,
        Config.AttunementTimeoutSeconds, false);
}

void UBindingCeremonySubsystem::OnPulseTick()
{
    NotifyPulseBeat();
}

void UBindingCeremonySubsystem::NotifyPulseBeat()
{
    if (!IsCeremonyActive() || Active.Phase != EBindingPhase::Attunement)
    {
        return;
    }
    Active.LastBeatTime = GetWorld()->GetTimeSeconds();
}

void UBindingCeremonySubsystem::TrySyncPulse()
{
    if (!IsCeremonyActive() || Active.Phase != EBindingPhase::Attunement)
    {
        return;
    }

    Active.PulseAttempts++;

    const float Now = GetWorld()->GetTimeSeconds();
    const bool bOnBeat = Active.LastBeatTime >= 0.0f
        && FMath::Abs(Now - Active.LastBeatTime) <= Config.SyncWindowSeconds;

    // The attempt itself costs: reaching out across the link is effort, and
    // the tamer feels it. A miss costs the strain but builds nothing.
    PayStrain(Config.StrainPerSyncAttempt);

    if (bOnBeat)
    {
        Active.PulseSyncs++;
        OnPulseSynced.Broadcast(Active.PulseSyncs, Config.RequiredPulseSyncs);

        if (Active.PulseSyncs >= Config.RequiredPulseSyncs)
        {
            GetWorld()->GetTimerManager().ClearTimer(PulseTimerHandle);
            GetWorld()->GetTimerManager().ClearTimer(PhaseTimerHandle);
            SetPhase(EBindingPhase::TheOffer);
        }
    }
}

void UBindingCeremonySubsystem::OnAttunementTimeout()
{
    if (!IsCeremonyActive() || Active.Phase != EBindingPhase::Attunement)
    {
        return;
    }
    // The rhythm never landed. The wild one drifts off; no harm, no claim.
    FinishCeremony(EBindingOutcome::Declined);
}

void UBindingCeremonySubsystem::OfferPurification()
{
    if (!IsCeremonyActive()
        || (Active.Phase != EBindingPhase::Attunement && Active.Phase != EBindingPhase::TheOffer))
    {
        return;
    }

    const float Corruption = GetCorruptionLevel();
    if (Corruption < Config.PurificationGateLevel)
    {
        return;
    }

    if (!PayStrain(Config.PurificationStrainCost))
    {
        return;
    }

    if (UWorld* World = GetWorld())
    {
        if (UCorruptionSubsystem* Corrupt = World->GetSubsystem<UCorruptionSubsystem>())
        {
            Corrupt->Purify(Active.EcoKinID, Config.PurificationAmount, TEXT("BindingCeremony"));
        }
    }
    Active.bPurifiedDuringCeremony = true;
}

void UBindingCeremonySubsystem::ExtendOffer()
{
    if (!IsCeremonyActive() || Active.Phase != EBindingPhase::TheOffer)
    {
        return;
    }

    // The link is bidirectional: if it cannot bear the alignment cost, the
    // ceremony holds here. Rest first, or step back. Never forced through.
    if (IsStrainBlocked())
    {
        if (!Active.bStrainHoldNotified)
        {
            Active.bStrainHoldNotified = true;
            OnStrainBlocked.Broadcast();
        }
        return;
    }
    Active.bStrainHoldNotified = false;

    // A corrupted wild one cannot accept until purification is offered.
    // Purification is part of the meeting, not a separate errand.
    if (GetCorruptionLevel() >= Config.PurificationGateLevel && !Active.bPurifiedDuringCeremony)
    {
        // The offer is extended but the wild one cannot answer yet: the
        // tamer must offer purification first. The ceremony holds at TheOffer.
        return;
    }

    LastOfferTimeByEcoKinID.Add(Active.EcoKinID, GetWorld()->GetTimeSeconds());

    GetWorld()->GetTimerManager().SetTimer(
        PhaseTimerHandle, this, &UBindingCeremonySubsystem::OnOfferDeliberated,
        Config.OfferDeliberationSeconds, false);
}

void UBindingCeremonySubsystem::OnOfferDeliberated()
{
    if (!IsCeremonyActive() || Active.Phase != EBindingPhase::TheOffer)
    {
        return;
    }

    const float Willingness = ComputeWillingness();

    FBindingRecord Record;
    Record.EcoKinID = Active.EcoKinID;
    Record.WildState = Active.WildState;
    Record.AttunementQuality = GetAttunementQuality();
    Record.WillingnessAtOffer = Willingness;
    Record.bPurifiedDuringCeremony = Active.bPurifiedDuringCeremony;

    if (Willingness < Config.WillingnessThreshold)
    {
        // The EcoKin chose no. It withdraws with dignity; the tamer keeps no claim.
        Record.Outcome = EBindingOutcome::Declined;
        Record.CeremonyDurationSeconds = GetWorld()->GetTimeSeconds() - Active.CeremonyStartTime;
        BindingHistory.Add(Record);
        if (UEncounterSubsystem* Encounters = GetEncounterSubsystem())
        {
            Encounters->EndEncounter(Active.WildEcoKin.Get(), EEncounterResolution::Withdrawn);
        }
        FinishCeremony(EBindingOutcome::Declined);
        return;
    }

    // Acceptance: the Anima-Link forms. The tamer pays the cost and feels it.
    SetPhase(EBindingPhase::Alignment);
    PayStrain(Config.AlignmentStrainCost);

    AEcoKinCompanion* WildOne = Active.WildEcoKin.Get();
    AActor* Tamer = Active.Tamer.Get();
    if (WildOne && Tamer)
    {
        WildOne->SetBondedTamer(Tamer);

        // Kindling starts here: the ceremony WAS the first trust. It starts
        // present but small; trust is earned, not granted.
        const float TrustSeed = Config.KindlingSeedBase + GetAttunementQuality() * Config.KindlingSeedAttunementBonus;
        if (UWorld* World = GetWorld())
        {
            if (UGameInstance* GI = World->GetGameInstance())
            {
                if (UKindlingProgressionSubsystem* Kindling = GI->GetSubsystem<UKindlingProgressionSubsystem>())
                {
                    FKindlingState Seed;
                    Seed.TrustPercent = TrustSeed;
                    Seed.Clamp();
                    Kindling->SetKindling(Active.EcoKinID, Seed);
                    Record.TrustSeeded = Seed.TrustPercent;
                }
            }
        }
    }

    Record.Outcome = EBindingOutcome::Aligned;
    Record.CeremonyDurationSeconds = GetWorld()->GetTimeSeconds() - Active.CeremonyStartTime;
    BindingHistory.Add(Record);

    if (UEncounterSubsystem* Encounters = GetEncounterSubsystem())
    {
        Encounters->EndEncounter(Active.WildEcoKin.Get(), EEncounterResolution::Bound);
    }
    FinishCeremony(EBindingOutcome::Aligned);
}

float UBindingCeremonySubsystem::ComputeWillingness() const
{
    float Base = 0.5f;
    if (const float* Found = Config.BaseWillingnessByState.Find(Active.WildState))
    {
        Base = *Found;
    }

    float Willingness = Base + GetAttunementQuality() * Config.AttunementWeight;

    // The wild one remembers kindness: peaceful past meetings count.
    Willingness += FMath::Min(Config.MemoryBonusCap,
        static_cast<float>(CountPeacefulMeetings(Active.EcoKinID)) * Config.MemoryBonusPerMeeting);

    // Corruption weighs the choice down. Purification during the ceremony lifts it.
    Willingness -= (GetCorruptionLevel() / 100.0f) * Config.CorruptionWeight;
    if (Active.bPurifiedDuringCeremony)
    {
        Willingness += Config.PurificationWillingnessBonus;
    }

    return FMath::Clamp(Willingness, 0.0f, 1.0f);
}

float UBindingCeremonySubsystem::GetCorruptionLevel() const
{
    if (UWorld* World = GetWorld())
    {
        if (UCorruptionSubsystem* Corrupt = World->GetSubsystem<UCorruptionSubsystem>())
        {
            return Corrupt->GetCorruptionLevel(Active.EcoKinID);
        }
    }
    return 0.0f;
}

int32 UBindingCeremonySubsystem::CountPeacefulMeetings(int32 EcoKinID) const
{
    int32 Count = 0;
    if (UEncounterSubsystem* Encounters = GetEncounterSubsystem())
    {
        for (const FEncounterRecord& Record : Encounters->EncounterHistory)
        {
            if (Record.EcoKinID == EcoKinID
                && (Record.Resolution == EEncounterResolution::Befriended
                    || Record.Resolution == EEncounterResolution::Withdrawn))
            {
                Count++;
            }
        }
    }
    return Count;
}

bool UBindingCeremonySubsystem::PayStrain(float Amount)
{
    AActor* Tamer = Active.Tamer.Get();
    if (!Tamer)
    {
        return false;
    }
    UAnimaLinkComponent* Link = Tamer->FindComponentByClass<UAnimaLinkComponent>();
    if (!Link)
    {
        return false;
    }
    // Tamer-side overexertion: the link strains in return. Bidirectional, always.
    Link->NotifyTamerOverexertion(Amount);
    return true;
}

bool UBindingCeremonySubsystem::IsStrainBlocked() const
{
    AActor* Tamer = Active.Tamer.Get();
    if (!Tamer)
    {
        return true;
    }
    const UAnimaLinkComponent* Link = Tamer->FindComponentByClass<UAnimaLinkComponent>();
    return !Link || Link->LinkState.IsFractureRisk();
}

void UBindingCeremonySubsystem::WithdrawOffer()
{
    if (!IsCeremonyActive())
    {
        return;
    }
    // Step back. Always allowed, never punished, no claim kept.
    if (UEncounterSubsystem* Encounters = GetEncounterSubsystem())
    {
        Encounters->EndEncounter(Active.WildEcoKin.Get(), EEncounterResolution::Withdrawn);
    }
    FinishCeremony(EBindingOutcome::WithdrawnByTamer);
}

void UBindingCeremonySubsystem::InterruptCeremony()
{
    if (!IsCeremonyActive())
    {
        return;
    }
    // Danger broke the rite. The wild one flees; no penalty, no bond, no grudge.
    if (UEncounterSubsystem* Encounters = GetEncounterSubsystem())
    {
        Encounters->EndEncounter(Active.WildEcoKin.Get(), EEncounterResolution::Fled);
    }
    FinishCeremony(EBindingOutcome::Interrupted);
}

void UBindingCeremonySubsystem::FinishCeremony(EBindingOutcome Outcome)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(PulseTimerHandle);
        World->GetTimerManager().ClearTimer(PhaseTimerHandle);
    }

    Active.WildEcoKin.Reset();
    Active.Tamer.Reset();
    Active.Phase = EBindingPhase::None;

    RestoreGamePhase();
    OnCeremonyCompleted.Broadcast(Outcome);
}

void UBindingCeremonySubsystem::RestoreGamePhase()
{
    if (UWorld* World = GetWorld())
    {
        if (AEchoheartsGameMode* GM = World->GetAuthGameMode<AEchoheartsGameMode>())
        {
            GM->SetPhase(EGamePhase::Exploration);
        }
    }
}

EBindingPhase UBindingCeremonySubsystem::GetCurrentPhase() const
{
    return Active.Phase;
}

bool UBindingCeremonySubsystem::IsCeremonyActive() const
{
    return Active.Phase != EBindingPhase::None
        && Active.WildEcoKin.IsValid()
        && Active.Tamer.IsValid();
}

float UBindingCeremonySubsystem::GetAttunementQuality() const
{
    if (Active.PulseAttempts <= 0)
    {
        return 0.0f;
    }
    // Softened ratio: early misses do not doom the ceremony.
    const float Raw = static_cast<float>(Active.PulseSyncs) / static_cast<float>(Active.PulseAttempts);
    return FMath::Clamp(Raw * 1.15f, 0.0f, 1.0f);
}

float UBindingCeremonySubsystem::ReadWillingness() const
{
    if (!IsCeremonyActive())
    {
        return 0.0f;
    }
    return ComputeWillingness();
}
