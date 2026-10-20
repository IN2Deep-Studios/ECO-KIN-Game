// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Battle/BattleSubsystem.h"
#include "Tamer/FrequencyTamerCharacter.h"
#include "Companion/EcoKinCompanion.h"
#include "EcoKin/KindlingProgressionSubsystem.h"
#include "AnimaLink/AnimaLinkTypes.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UBattleSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    RegisterChainDefs();
    bBattleActive = false;
    bClimaxActive = false;
}

void UBattleSubsystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!bBattleActive)
    {
        return;
    }

    // Momentum decays when the bond goes quiet. Relationships cool.
    const float Now = GetWorldTime();
    for (TPair<int32, FHarmonicMomentum>& Pair : MomentumMap)
    {
        FHarmonicMomentum& M = Pair.Value;
        if (Now - LastPulseTime > M.PulseInterval * 2.0f && M.Momentum > 0.0f)
        {
            M.Momentum = FMath::Max(0.0f, M.Momentum - DeltaTime * 4.0f);
            if (M.Momentum <= 0.0f)
            {
                M.PulseStreak = 0;
            }
        }
    }

    UpdateClimax();
}

TStatId UBattleSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UBattleSubsystem, STATGROUP_Tickables);
}

bool UBattleSubsystem::StartDuel(AActor* ChallengerTamer, AActor* OpponentTamer)
{
    if (bBattleActive || !ChallengerTamer || !OpponentTamer)
    {
        return false;
    }
    // One deployed EcoKin per side — the duel is fought by partners, not armies.
    if (GetDeployedEcoKinID(ChallengerTamer) <= 0 || GetDeployedEcoKinID(OpponentTamer) <= 0)
    {
        return false;
    }
    bBattleActive = true;
    ActiveMode = EBattleMode::Duel;
    DuelTamers = { ChallengerTamer, OpponentTamer };
    LastPulseTime = GetWorldTime();
    return true;
}

bool UBattleSubsystem::StartRaid(const TArray<AActor*>& ParticipantTamers, FName BossId, AActor* RaidTarget)
{
    if (bBattleActive || ParticipantTamers.Num() == 0 || !RaidTarget || BossId.IsNone())
    {
        return false;
    }
    // Every participant brings their one deployed partner. The raid is won
    // by coordination — essence chains across Tamers — not by numbers alone.
    for (AActor* Tamer : ParticipantTamers)
    {
        if (!Tamer || GetDeployedEcoKinID(Tamer) <= 0)
        {
            return false;
        }
    }
    bBattleActive = true;
    ActiveMode = EBattleMode::Raid;
    DuelTamers.Empty();
    for (AActor* Tamer : ParticipantTamers)
    {
        DuelTamers.Add(Tamer);
    }
    LastPulseTime = GetWorldTime();
    // NOTE: networked raid parties arrive with the server layer. Today,
    // participants are local/AI allies; the API already takes a party array.
    return true;
}

bool UBattleSubsystem::StartSparring(AActor* TamerA, AActor* TamerB)
{
    if (bBattleActive || !TamerA || !TamerB)
    {
        return false;
    }
    if (GetDeployedEcoKinID(TamerA) <= 0 || GetDeployedEcoKinID(TamerB) <= 0)
    {
        return false;
    }
    // Sparring is consensual by definition — both Tamers agreed to be here.
    bBattleActive = true;
    ActiveMode = EBattleMode::Sparring;
    DuelTamers = { TamerA, TamerB };
    LastPulseTime = GetWorldTime();
    return true;
}

void UBattleSubsystem::EndBattle(EBattleResult Result)
{
    if (!bBattleActive)
    {
        return;
    }

    // Sparring pays out in trust, not wounds. Both partners fought together —
    // the bond deepens whether they "won" or not.
    if (ActiveMode == EBattleMode::Sparring &&
        (Result == EBattleResult::Victory || Result == EBattleResult::Defeat || Result == EBattleResult::Draw))
    {
        if (UGameInstance* GI = GetGameInstance())
        {
            if (UKindlingProgressionSubsystem* Kindling = GI->GetSubsystem<UKindlingProgressionSubsystem>())
            {
                for (const TWeakObjectPtr<AActor>& TamerPtr : DuelTamers)
                {
                    if (AActor* Tamer = TamerPtr.Get())
                    {
                        const int32 EcoKinID = GetDeployedEcoKinID(Tamer);
                        if (EcoKinID > 0)
                        {
                            Kindling->RecordTrustEvent(EcoKinID, EKindlingTrustEvent::BattleFoughtTogether);
                        }
                    }
                }
            }
        }
    }

    bBattleActive = false;
    if (bClimaxActive)
    {
        bClimaxActive = false;
        OnResonantClimaxChanged.Broadcast(false);
    }
    MomentumMap.Empty();
    ChainMap.Empty();
    DuelTamers.Empty();
}

bool UBattleSubsystem::NotifyAbilityUsed(AActor* Wielder, EEcoKinEssence Essence, AActor* Target, FName& ChainIdOut)
{
    ChainIdOut = NAME_None;
    if (!bBattleActive || !Wielder)
    {
        return false;
    }

    AEcoKinCompanion* Companion = Cast<AEcoKinCompanion>(Wielder);
    if (!Companion || Companion->EcoKinID <= 0)
    {
        return false;
    }
    const int32 EcoKinID = Companion->EcoKinID;
    const float Now = GetWorldTime();

    // Harmonic Momentum: was this activation on the link's pulse?
    FHarmonicMomentum& M = MomentumMap.FindOrAdd(EcoKinID);
    const float SincePulse = FMath::Fmod(Now - LastPulseTime, M.PulseInterval);
    const float Window = bClimaxActive ? M.PulseWindow * 2.0f : M.PulseWindow;
    const bool bOnPulse = SincePulse <= Window || (M.PulseInterval - SincePulse) <= Window;
    UpdateMomentum(EcoKinID, bOnPulse);

    // Essence chaining: rapid different-essence strikes combine.
    FEssenceChain& Chain = ChainMap.FindOrAdd(EcoKinID);
    const bool bChained = CheckChain(Essence, Chain, ChainIdOut);
    Chain.LastEssence = Essence;
    Chain.LastStrikeTime = Now;
    Chain.bHasFirst = true;

    if (bChained && !ChainIdOut.IsNone())
    {
        OnChainTriggered.Broadcast(ChainIdOut, Wielder, Target);
        // A spent chain resets — chains are earned fresh, every time.
        Chain.bHasFirst = false;
        return true;
    }
    return false;
}

float UBattleSubsystem::GetMomentum(int32 EcoKinID) const
{
    const FHarmonicMomentum* M = MomentumMap.Find(EcoKinID);
    return M ? M->Momentum : 0.0f;
}

void UBattleSubsystem::RegisterChainDefs()
{
    ChainDefs.Empty();

    // Six essence chains. Each is two essences in sequence within the
    // chain window — coordination made manifest.
    auto Add = [this](const char* Id, const char* Name, EEcoKinEssence A, EEcoKinEssence B, const char* Effect)
    {
        FEssenceChainDef D;
        D.ChainId = FName(Id);
        D.ChainName = Name;
        D.FirstEssence = A;
        D.SecondEssence = B;
        D.EffectDescription = Effect;
        ChainDefs.Add(D);
    };

    Add("OverchargeFlash", "Overcharge Flash", EEcoKinEssence::Torrent, EEcoKinEssence::Voltic,
        "Water carries the current straight in — the target staggers, its next action interrupted.");
    Add("WildfireSurge", "Wildfire Surge", EEcoKinEssence::Pyre, EEcoKinEssence::Aero,
        "Wind feeds the flame into a surging burn that spreads across the arena floor.");
    Add("Shatterlock", "Shatterlock", EEcoKinEssence::Glaze, EEcoKinEssence::Terra,
        "Frost makes brittle what stone then breaks — armor cracked, defense lowered.");
    Add("Bloomtide", "Bloomtide", EEcoKinEssence::Flora, EEcoKinEssence::Torrent,
        "Growth rides the water — a restorative tide that eases allies' strain and mends wounds.");
    Add("EclipseVeil", "Eclipse Veil", EEcoKinEssence::Aura, EEcoKinEssence::Shade,
        "Light swallowed whole — the target's senses scatter, its aim falters.");
    Add("Stormglass", "Stormglass", EEcoKinEssence::Voltic, EEcoKinEssence::Glaze,
        "Lightning frozen mid-strike — chaining arcs that slow everything they touch.");
}

bool UBattleSubsystem::CheckChain(EEcoKinEssence Essence, FEssenceChain& Chain, FName& ChainIdOut) const
{
    if (!Chain.bHasFirst)
    {
        return false;
    }
    const float Now = GetWorldTime();
    for (const FEssenceChainDef& Def : ChainDefs)
    {
        if (Def.FirstEssence == Chain.LastEssence && Def.SecondEssence == Essence &&
            (Now - Chain.LastStrikeTime) <= Def.ChainWindow)
        {
            ChainIdOut = Def.ChainId;
            return true;
        }
    }
    return false;
}

void UBattleSubsystem::UpdateMomentum(int32 EcoKinID, bool bOnPulse)
{
    FHarmonicMomentum* M = MomentumMap.Find(EcoKinID);
    if (!M)
    {
        return;
    }
    if (bOnPulse)
    {
        M->PulseStreak++;
        // Streaks accelerate: the deeper the rhythm, the faster it builds.
        const float Gain = bClimaxActive ? 16.0f : 8.0f + FMath::Min(8.0f, M->PulseStreak * 1.5f);
        M->Momentum = FMath::Min(100.0f, M->Momentum + Gain);
    }
    else
    {
        M->PulseStreak = 0;
        M->Momentum = FMath::Max(0.0f, M->Momentum - 6.0f);
    }
}

void UBattleSubsystem::UpdateClimax()
{
    // Resonant Climax: when both bonds are burning hot (strain above
    // threshold), the arena itself resonates. Pulse windows double and
    // momentum builds twice as fast — but everything costs more strain.
    // Intensity has a price; the bond pays it together.
    bool bShouldClimax = false;
    if (DuelTamers.Num() >= 2)
    {
        bShouldClimax = true;
        for (const TWeakObjectPtr<AActor>& TamerPtr : DuelTamers)
        {
            if (AActor* Tamer = TamerPtr.Get())
            {
                if (GetLinkStrain(Tamer) < ClimaxStrainThreshold)
                {
                    bShouldClimax = false;
                    break;
                }
            }
            else
            {
                bShouldClimax = false;
                break;
            }
        }
    }

    if (bShouldClimax != bClimaxActive)
    {
        bClimaxActive = bShouldClimax;
        OnResonantClimaxChanged.Broadcast(bClimaxActive);
    }
}

float UBattleSubsystem::GetLinkStrain(AActor* Tamer) const
{
    if (AFrequencyTamerCharacter* C = Cast<AFrequencyTamerCharacter>(Tamer))
    {
        if (C->AnimaLink)
        {
            return C->AnimaLink->LinkState.LinkStrain;
        }
    }
    return 0.0f;
}

int32 UBattleSubsystem::GetDeployedEcoKinID(AActor* Tamer) const
{
    if (AFrequencyTamerCharacter* C = Cast<AFrequencyTamerCharacter>(Tamer))
    {
        if (AEcoKinCompanion* Comp = Cast<AEcoKinCompanion>(C->ActiveCompanion))
        {
            return Comp->EcoKinID;
        }
        return 0;
    }
    // Generic NPC duelist: companion actor tagged with an EcoKin ID.
    if (AEcoKinCompanion* Comp = Cast<AEcoKinCompanion>(Tamer))
    {
        return Comp->EcoKinID;
    }
    return 0;
}

float UBattleSubsystem::GetWorldTime() const
{
    if (UWorld* W = GetWorld())
    {
        return W->GetTimeSeconds();
    }
    return 0.0f;
}
