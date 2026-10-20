// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "EcoKin/RiteDefenseSubsystem.h"

void URiteDefenseSubsystem::RegisterDefense(const FRiteDefenseDef& Def)
{
    if (Def.DefenseId.IsNone() || Def.HoldDurationSeconds <= 0.0f)
    {
        return;
    }
    Defenses.Add(Def.DefenseId, Def);
    States.Add(Def.DefenseId, ERiteDefenseState::Inactive);
}

bool URiteDefenseSubsystem::BeginDefense(FName DefenseId)
{
    if (!Defenses.Contains(DefenseId))
    {
        return false;
    }
    ERiteDefenseState* State = States.Find(DefenseId);
    if (!State || *State == ERiteDefenseState::Active || *State == ERiteDefenseState::Preparing)
    {
        return false;
    }

    *State = ERiteDefenseState::Active;
    ElapsedSeconds.Add(DefenseId, 0.0f);
    NextWaveIndex.Add(DefenseId, 0);
    Breaches.Add(DefenseId, 0);

    OnDefenseStarted.Broadcast(DefenseId);

    if (UWorld* World = GetWorld())
    {
        FTimerDelegate TickDelegate = FTimerDelegate::CreateUObject(this, &URiteDefenseSubsystem::TickDefense, DefenseId);
        World->GetTimerManager().SetTimer(DefenseTimerHandle, TickDelegate, 1.0f, true);
    }
    return true;
}

void URiteDefenseSubsystem::ReportIntruderResolved(FName DefenseId, bool bKeptOut)
{
    if (!bKeptOut)
    {
        int32* Count = Breaches.Find(DefenseId);
        if (Count)
        {
            ++(*Count);
        }
    }
}

void URiteDefenseSubsystem::EndDefense(FName DefenseId)
{
    ERiteDefenseState* State = States.Find(DefenseId);
    if (State && (*State == ERiteDefenseState::Active || *State == ERiteDefenseState::Preparing))
    {
        FinishDefense(DefenseId, false);
    }
}

ERiteDefenseState URiteDefenseSubsystem::GetDefenseState(FName DefenseId) const
{
    const ERiteDefenseState* State = States.Find(DefenseId);
    return State ? *State : ERiteDefenseState::Inactive;
}

void URiteDefenseSubsystem::TickDefense(FName DefenseId)
{
    ERiteDefenseState* State = States.Find(DefenseId);
    const FRiteDefenseDef* Def = Defenses.Find(DefenseId);
    float* Elapsed = ElapsedSeconds.Find(DefenseId);
    int32* WaveIdx = NextWaveIndex.Find(DefenseId);
    if (!State || !Def || !Elapsed || !WaveIdx || *State != ERiteDefenseState::Active)
    {
        return;
    }

    *Elapsed += 1.0f;

    while (*WaveIdx < Def->Waves.Num() && Def->Waves[*WaveIdx].ArriveAtSeconds <= *Elapsed)
    {
        OnDefenseWave.Broadcast(DefenseId, *WaveIdx);
        ++(*WaveIdx);
    }

    if (*Elapsed >= Def->HoldDurationSeconds)
    {
        // The circle held if nothing breached it. A broken circle pauses
        // the rite — retry, no penalty beyond time.
        const int32* BreachCount = Breaches.Find(DefenseId);
        FinishDefense(DefenseId, !BreachCount || *BreachCount == 0);
    }
}

void URiteDefenseSubsystem::FinishDefense(FName DefenseId, bool bCircleHeld)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(DefenseTimerHandle);
    }
    ERiteDefenseState* State = States.Find(DefenseId);
    if (State)
    {
        *State = bCircleHeld ? ERiteDefenseState::Complete : ERiteDefenseState::Broken;
    }
    ElapsedSeconds.Remove(DefenseId);
    NextWaveIndex.Remove(DefenseId);
    Breaches.Remove(DefenseId);
    OnDefenseCompleted.Broadcast(DefenseId, bCircleHeld);
}
