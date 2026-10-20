// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Progression/BossProgressionSubsystem.h"

void UBossProgressionSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    RegisterCanonicalBosses();
}

void UBossProgressionSubsystem::AddBoss(const TCHAR* Id, const TCHAR* Name, const TCHAR* Title,
    bool bRequired, int32 Order, int32 CorruptedDexID, const TCHAR* Hint,
    const TCHAR* TransitionId, const TCHAR* TransitionDesc)
{
    FBossDef Def;
    Def.BossId = Id;
    Def.Name = Name;
    Def.Title = Title;
    Def.bRequired = bRequired;
    Def.OrderIndex = Order;
    Def.CorruptedDexID = CorruptedDexID;
    Def.UnlockHint = Hint;
    Def.WorldTransitionId = TransitionId;
    Def.WorldTransitionDescription = TransitionDesc;
    RegisterBossDef(Def);
}

void UBossProgressionSubsystem::RegisterCanonicalBosses()
{
    // The fourteen, in order. Corrupted alphas carry their Twelve Wounded
    // Dex IDs; the Beasts stand on their own names; the society champions
    // are titled by role — their societies' true names are not yet known.
    AddBoss(TEXT("Gloomtrot"), TEXT("Gloomtrot"), TEXT("the Wrongward Courier"),
        false, 1, 498,
        TEXT("Something is running the Mist Coast routes that should not be. Follow the blighted messages."),
        TEXT(""), TEXT(""));
    AddBoss(TEXT("Rustmaul"), TEXT("Rustmaul"), TEXT("the Corroded Hoard"),
        true, 2, 503,
        TEXT("The Sunforge Gardens have gone quiet. The gold there no longer shines."),
        TEXT("SunforgeStirs"), TEXT("The Sunforge Gardens stir. Restoration work can begin there."));
    AddBoss(TEXT("Blightstorm"), TEXT("Blightstorm"), TEXT("the Blackened Sky"),
        true, 3, 502,
        TEXT("The storm belt rains black. Nothing flies until it is answered."),
        TEXT("SkiesClear"), TEXT("The skies clear. Flight routes open across the storm belt."));
    AddBoss(TEXT("Fulgurix"), TEXT("Fulgurix"), TEXT("Storm Raptor of the Highlands"),
        true, 4, 0,
        TEXT("The first of the Seven Beasts rides the highland thermals, waiting for a worthy sky."),
        TEXT("HighlandsOpen"), TEXT("The highland regions open. The Beasts have noticed you."));
    AddBoss(TEXT("Rotwarden"), TEXT("Rotwarden"), TEXT("the Strangling Canopy"),
        false, 5, 510,
        TEXT("The ancient groves are choking. Their protector is the thing choking them."),
        TEXT(""), TEXT(""));
    AddBoss(TEXT("Bastodon"), TEXT("Bastodon"), TEXT("the Walking Fortress"),
        true, 6, 0,
        TEXT("A fortress walks the overland roads. Caravans will not move until it does — or falls."),
        TEXT("CaravansMove"), TEXT("Overland caravan routes open. Supplies flow between Sanctuaries."));
    AddBoss(TEXT("Hadalon"), TEXT("Hadalon"), TEXT("the Abyssal Leviathan"),
        true, 7, 0,
        TEXT("The deep ocean has a heartbeat, and it is angry. Dive when you are ready."),
        TEXT("DeepOceanOpen"), TEXT("The deep ocean regions open. What sleeps below has woken."));
    AddBoss(TEXT("Kryodon"), TEXT("Kryodon"), TEXT("the Glacial Behemoth"),
        true, 8, 0,
        TEXT("The glacial rifts groan under something enormous. The ice remembers being afraid."),
        TEXT("RiftsSafe"), TEXT("The glacial rifts are safe to cross. The cold is only cold now."));
    AddBoss(TEXT("Pyrewyrm"), TEXT("Pyrewyrm"), TEXT("the Magma Wyrm"),
        true, 9, 0,
        TEXT("The Ashfall Spires burn without end. The wyrm at their heart must be faced."),
        TEXT("LandCools"), TEXT("The land cools. Blight recedes across the fire-scarred regions."));
    AddBoss(TEXT("SocietyHerald"), TEXT("Society Herald"), TEXT("Champion of the Secret Societies"),
        true, 10, 0,
        TEXT("They have been watching since the first Sanctuary rose. Now they send a herald."),
        TEXT("SocietyWar"), TEXT("The secret societies move openly. Sanctuaries must be defended."));
    AddBoss(TEXT("Nyctalon"), TEXT("Nyctalon"), TEXT("the Night Raptor"),
        false, 11, 0,
        TEXT("On moonless nights something hunts the dark between stars. It is beautiful. It is waiting."),
        TEXT(""), TEXT(""));
    AddBoss(TEXT("Grovantis"), TEXT("GROVANTIS"), TEXT("the Flora-Crystal Lion"),
        true, 12, 0,
        TEXT("The lion of living crystal walks the heartlands. It does not yield to the unproven."),
        TEXT("HeartlandsOpen"), TEXT("The flora-crystal heartlands open. The wilds themselves vouch for you."));
    AddBoss(TEXT("ShadowBeastPrime"), TEXT("Shadow Beast Prime"), TEXT("the Fractured Apex"),
        true, 13, 0,
        TEXT("Every fracture you never healed has been gathering. It has a shape now. It has a name."),
        TEXT("WorldBreathesEasier"),
        TEXT("The world breathes easier. Blight recedes across Rebearth; sealed regions open."));
    AddBoss(TEXT("SocietyParagon"), TEXT("Society Paragon"), TEXT("the Conqueror's Answer"),
        true, 14, 0,
        TEXT("The societies' final argument: that the living world is a weapon. Prove them wrong."),
        TEXT("RestorationEra"), TEXT("The Restoration Era begins. What was broken is being mended."));
}

void UBossProgressionSubsystem::RegisterBossDef(const FBossDef& Def)
{
    if (Def.BossId.IsNone())
    {
        return;
    }
    BossDefs.Add(Def.BossId, Def);
}

bool UBossProgressionSubsystem::CanChallenge(FName BossId) const
{
    const FBossDef* Def = BossDefs.Find(BossId);
    if (!Def || IsBossDefeated(BossId))
    {
        return false;
    }
    // Every REQUIRED boss earlier in the order must have fallen. Optional
    // bosses never gate anything — wander freely.
    for (const TPair<FName, FBossDef>& Pair : BossDefs)
    {
        if (Pair.Value.bRequired && Pair.Value.OrderIndex < Def->OrderIndex
            && !DefeatedBosses.Contains(Pair.Key))
        {
            return false;
        }
    }
    return true;
}

FString UBossProgressionSubsystem::MarkBossDefeated(FName BossId)
{
    const FBossDef* Def = BossDefs.Find(BossId);
    if (!Def || DefeatedBosses.Contains(BossId))
    {
        return TEXT("");
    }
    DefeatedBosses.Add(BossId);

    FString Summary = FString::Printf(TEXT("%s %s has fallen."), *Def->Name, *Def->Title);

    // Fire the world transition once. The world only changes the first time.
    if (!Def->WorldTransitionId.IsNone() && !FiredTransitions.Contains(Def->WorldTransitionId))
    {
        FiredTransitions.Add(Def->WorldTransitionId);
        Summary += TEXT(" ") + Def->WorldTransitionDescription;

        // Threshold bosses advance the era of the world itself.
        if (Def->WorldTransitionId == TEXT("SkiesClear"))
        {
            CurrentEra = EWorldEra::SkiesClear;
        }
        else if (Def->WorldTransitionId == TEXT("LandCools"))
        {
            CurrentEra = EWorldEra::LandCools;
        }
        else if (Def->WorldTransitionId == TEXT("WorldBreathesEasier"))
        {
            CurrentEra = EWorldEra::WorldBreathesEasier;
        }
        else if (Def->WorldTransitionId == TEXT("RestorationEra"))
        {
            CurrentEra = EWorldEra::RestorationEra;
        }
    }
    return Summary;
}

bool UBossProgressionSubsystem::IsBossDefeated(FName BossId) const
{
    return DefeatedBosses.Contains(BossId);
}

bool UBossProgressionSubsystem::TryGetBossDef(FName BossId, FBossDef& OutDef) const
{
    const FBossDef* Def = BossDefs.Find(BossId);
    if (!Def)
    {
        return false;
    }
    OutDef = *Def;
    return true;
}

bool UBossProgressionSubsystem::TryGetBossPhases(FName BossId, TArray<FBossPhaseDef>& OutPhases) const
{
    const FBossDef* Def = BossDefs.Find(BossId);
    if (!Def)
    {
        return false;
    }
    OutPhases = Def->Phases;
    return true;
}
