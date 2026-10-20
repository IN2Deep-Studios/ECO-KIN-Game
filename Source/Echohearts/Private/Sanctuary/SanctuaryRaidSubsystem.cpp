// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Sanctuary/SanctuaryRaidSubsystem.h"
#include "BaseBuilding/BaseBuildingSubsystem.h"

void USanctuaryRaidSubsystem::RegisterRaid(const FSanctuaryRaidDef& Def)
{
    if (Def.RaidId.IsNone() || Def.Waves.IsEmpty())
    {
        return;
    }
    Raids.Add(Def.RaidId, Def);
    States.Add(Def.RaidId, ERaidState::Quiet);
}

bool USanctuaryRaidSubsystem::TriggerRaid(FName RaidId, float DefenseRating)
{
    const FSanctuaryRaidDef* Def = Raids.Find(RaidId);
    ERaidState* State = States.Find(RaidId);
    if (!Def || !State || *State == ERaidState::Warned || *State == ERaidState::Underway)
    {
        return false;
    }

    // A well-fortified sanctuary breaks the raid before it begins: the
    // raiders see the Bulwark lit and think better of it.
    if (DefenseRating >= Def->RepelDefenseThreshold)
    {
        *State = ERaidState::Repelled;
        OnRaidEnded.Broadcast(RaidId, true);
        return true;
    }

    *State = ERaidState::Warned;
    CachedDefenseRating.Add(RaidId, DefenseRating);
    ElapsedSeconds.Add(RaidId, -Def->WarningSeconds); // negative = warning period
    NextWaveIndex.Add(RaidId, 0);
    ResolvedWaves.Add(RaidId, 0);
    OnRaidWarned.Broadcast(RaidId);

    if (UWorld* World = GetWorld())
    {
        FTimerDelegate TickDelegate = FTimerDelegate::CreateUObject(this, &USanctuaryRaidSubsystem::TickRaid, RaidId);
        World->GetTimerManager().SetTimer(RaidTimerHandle, TickDelegate, 1.0f, true);
    }
    return true;
}

void USanctuaryRaidSubsystem::ReportWaveResolved(FName RaidId)
{
    ERaidState* State = States.Find(RaidId);
    const FSanctuaryRaidDef* Def = Raids.Find(RaidId);
    int32* Resolved = ResolvedWaves.Find(RaidId);
    if (!State || !Def || !Resolved || *State != ERaidState::Underway)
    {
        return;
    }

    ++(*Resolved);
    if (*Resolved >= Def->Waves.Num())
    {
        // Every wave resolved: repelled. Flawless (no breaches tracked
        // here — the base structures softened it) earns the Warden track
        // via the OnRaidEnded listener.
        FinishRaid(RaidId, true);
    }
}

ERaidState USanctuaryRaidSubsystem::GetRaidState(FName RaidId) const
{
    const ERaidState* State = States.Find(RaidId);
    return State ? *State : ERaidState::Quiet;
}

float USanctuaryRaidSubsystem::GetRaidDamageMultiplier(FName RaidId) const
{
    const int32* Level = EscalationLevels.Find(RaidId);
    return 1.0f + 0.25f * static_cast<float>(Level ? *Level : 0);
}

void USanctuaryRaidSubsystem::TickRaid(FName RaidId)
{
    ERaidState* State = States.Find(RaidId);
    const FSanctuaryRaidDef* Def = Raids.Find(RaidId);
    float* Elapsed = ElapsedSeconds.Find(RaidId);
    int32* WaveIdx = NextWaveIndex.Find(RaidId);
    if (!State || !Def || !Elapsed || !WaveIdx)
    {
        return;
    }

    *Elapsed += 1.0f;

    if (*State == ERaidState::Warned && *Elapsed >= 0.0f)
    {
        *State = ERaidState::Underway;
        BeginWaves(RaidId);
    }

    if (*State != ERaidState::Underway)
    {
        return;
    }

    while (*WaveIdx < Def->Waves.Num() && Def->Waves[*WaveIdx].ArriveAtSeconds <= *Elapsed)
    {
        OnRaidWave.Broadcast(RaidId, *WaveIdx);
        ++(*WaveIdx);
    }

    // Unresolved waves break things. Every interval, each arrived-but-
    // unresolved wave damages a standing structure. Go hard or lose.
    const int32 ArrivedCount = *WaveIdx;
    const int32* Resolved = ResolvedWaves.Find(RaidId);
    const int32 Unresolved = ArrivedCount - (Resolved ? *Resolved : 0);
    if (Unresolved > 0)
    {
        float* Clock = DamageClocks.Find(RaidId);
        if (!Clock)
        {
            DamageClocks.Add(RaidId, 0.0f);
            Clock = DamageClocks.Find(RaidId);
        }
        *Clock += 1.0f;
        if (*Clock >= Def->DamageIntervalSeconds)
        {
            *Clock = 0.0f;
            ApplyRaidDamage(RaidId, Unresolved);
        }
    }
}

void USanctuaryRaidSubsystem::BeginWaves(FName RaidId)
{
    // Waves dispatch through TickRaid; this marks the transition so the
    // Sanctuary (and its residents) can take positions.
}

void USanctuaryRaidSubsystem::ApplyRaidDamage(FName RaidId, int32 UnresolvedWaves)
{
    const FSanctuaryRaidDef* Def = Raids.Find(RaidId);
    UGameInstance* GI = GetGameInstance();
    if (!Def || !GI || UnresolvedWaves <= 0)
    {
        return;
    }
    UBaseBuildingSubsystem* Base = GI->GetSubsystem<UBaseBuildingSubsystem>();
    if (!Base)
    {
        return;
    }

    TArray<FName> Standing = Base->GetStandingStructureIds();
    if (Standing.IsEmpty())
    {
        return;
    }

    // Raiders hit the walls first: prefer defense structures, else anything
    // standing. Each unresolved wave deals its share.
    const float Damage = Def->StructureDamagePerInterval * GetRaidDamageMultiplier(RaidId);
    for (int32 i = 0; i < UnresolvedWaves && !Standing.IsEmpty(); ++i)
    {
        const FName Target = Standing[FMath::RandRange(0, Standing.Num() - 1)];
        const bool bDestroyed = Base->DamageStructure(Target, Damage);
        if (bDestroyed)
        {
            Standing.Remove(Target);
        }

        // The Spire falls: the base is overrun. Everything else is rubble
        // politics — end it now.
        if (Target == Def->AnchorStructureId && bDestroyed)
        {
            FinishRaid(RaidId, false);
            return;
        }
    }
}

void USanctuaryRaidSubsystem::FinishRaid(FName RaidId, bool bRepelled)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(RaidTimerHandle);
    }
    ERaidState* State = States.Find(RaidId);
    if (State)
    {
        *State = bRepelled ? ERaidState::Repelled : ERaidState::Breached;
    }
    if (!bRepelled)
    {
        // The societies remember. Next time hits 25% harder.
        int32* Level = EscalationLevels.Find(RaidId);
        EscalationLevels.Add(RaidId, (Level ? *Level : 0) + 1);
    }
    ElapsedSeconds.Remove(RaidId);
    NextWaveIndex.Remove(RaidId);
    ResolvedWaves.Remove(RaidId);
    CachedDefenseRating.Remove(RaidId);
    DamageClocks.Remove(RaidId);
    OnRaidEnded.Broadcast(RaidId, bRepelled);
}
