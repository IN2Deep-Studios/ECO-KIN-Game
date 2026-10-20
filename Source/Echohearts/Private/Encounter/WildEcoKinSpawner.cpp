// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Encounter/WildEcoKinSpawner.h"
#include "Companion/EcoKinCompanion.h"
#include "Encounter/WildBehaviorComponent.h"
#include "EcoKin/EcoKinDexSubsystem.h"
#include "Corruption/CorruptionSubsystem.h"
#include "Corruption/CorruptionTypes.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"

AWildEcoKinSpawner::AWildEcoKinSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
    RootComponent = SpawnVolume;
    SpawnVolume->SetBoxExtent(FVector(2000.0f, 2000.0f, 500.0f));
}

void AWildEcoKinSpawner::BeginPlay()
{
    Super::BeginPlay();
    if (SpawnTable.SpawnIntervalSeconds > 0.0f && !SpawnTable.Entries.IsEmpty())
    {
        GetWorldTimerManager().SetTimer(SpawnTimerHandle, this,
            &AWildEcoKinSpawner::SpawnTick, SpawnTable.SpawnIntervalSeconds, true);
    }
}

void AWildEcoKinSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
    Super::EndPlay(EndPlayReason);
}

int32 AWildEcoKinSpawner::GetLivingWildCount() const
{
    int32 Count = 0;
    for (const TWeakObjectPtr<AEcoKinCompanion>& Wild : LivingWild)
    {
        if (Wild.IsValid())
        {
            ++Count;
        }
    }
    return Count;
}

void AWildEcoKinSpawner::AddExclusiveSpecies(int32 EcoKinID, float Weight)
{
    if (EcoKinID <= 0 || Weight <= 0.0f)
    {
        return;
    }
    for (const FEcoSpawnEntry& Entry : SpawnTable.Entries)
    {
        if (Entry.EcoKinID == EcoKinID)
        {
            return; // Already in the wild population. Exclusivity is a beginning, not a duplicate.
        }
    }

    FEcoSpawnEntry Exclusive;
    Exclusive.EcoKinID = EcoKinID;
    Exclusive.Weight = Weight;
    // Exclusives enter the wild calm and solitary: they were just welcomed,
    // not hunted. Rage/Blight/Hollow states can still apply per encounter rules.
    Exclusive.AllowedStates = { EEncounterState::Calm };
    Exclusive.MaturityMin = 0.3f;
    Exclusive.MaturityMax = 0.7f;
    Exclusive.PackMin = 1;
    Exclusive.PackMax = 1;
    SpawnTable.Entries.Add(Exclusive);
}

void AWildEcoKinSpawner::SpawnTick()
{
    PruneDistant();
    if (GetLivingWildCount() < SpawnTable.MaxActivePopulation)
    {
        TrySpawnOne();
    }
}

bool AWildEcoKinSpawner::IsKnownSpecies(int32 EcoKinID) const
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UEcoKinDexSubsystem* Dex = GI->GetSubsystem<UEcoKinDexSubsystem>())
        {
            FEcoKinIdentity Identity;
            return Dex->TryGetIdentity(EcoKinID, Identity);
        }
    }
    // No Dex loaded (early prototype): trust designer data.
    return true;
}

const FEcoSpawnEntry* AWildEcoKinSpawner::RollEntry() const
{
    float TotalWeight = 0.0f;
    for (const FEcoSpawnEntry& Entry : SpawnTable.Entries)
    {
        TotalWeight += FMath::Max(0.0f, Entry.Weight);
    }
    if (TotalWeight <= 0.0f)
    {
        return nullptr;
    }

    float Roll = FMath::FRand() * TotalWeight;
    for (const FEcoSpawnEntry& Entry : SpawnTable.Entries)
    {
        Roll -= FMath::Max(0.0f, Entry.Weight);
        if (Roll <= 0.0f)
        {
            return &Entry;
        }
    }
    return &SpawnTable.Entries.Last();
}

EEncounterState AWildEcoKinSpawner::RollState(const FEcoSpawnEntry& Entry) const
{
    if (Entry.AllowedStates.IsEmpty())
    {
        return EEncounterState::Calm;
    }

    // Nature rule and its kin: some Dex IDs never roll Alpha. The list is
    // designer data; Nature is the canonical entry.
    EEncounterState Rolled = Entry.AllowedStates[FMath::RandRange(0, Entry.AllowedStates.Num() - 1)];
    if (Rolled == EEncounterState::Alpha && NeverAlphaIDs.Contains(Entry.EcoKinID))
    {
        return EEncounterState::Calm;
    }
    return Rolled;
}

FVector AWildEcoKinSpawner::RollSpawnLocation() const
{
    const FVector Center = SpawnVolume->GetComponentLocation();
    const FVector Extent = SpawnVolume->GetScaledBoxExtent();
    return Center + FVector(FMath::FRandRange(-Extent.X, Extent.X),
                           FMath::FRandRange(-Extent.Y, Extent.Y),
                           FMath::FRandRange(-Extent.Z, Extent.Z));
}

bool AWildEcoKinSpawner::TrySpawnOne()
{
    if (!WildEcoKinClass)
    {
        return false;
    }

    const FEcoSpawnEntry* Entry = RollEntry();
    if (!Entry || !IsKnownSpecies(Entry->EcoKinID))
    {
        return false;
    }

    const EEncounterState State = RollState(*Entry);
    // Alphas walk alone. Always.
    const int32 PackSize = (State == EEncounterState::Alpha)
        ? 1
        : FMath::RandRange(Entry->PackMin, FMath::Max(Entry->PackMin, Entry->PackMax));

    for (int32 i = 0; i < PackSize; ++i)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AEcoKinCompanion* Wild = GetWorld()->SpawnActor<AEcoKinCompanion>(
            WildEcoKinClass, RollSpawnLocation(), FRotator::ZeroRotator, Params);
        if (!Wild)
        {
            continue;
        }

        Wild->EcoKinID = Entry->EcoKinID;
        Wild->SpawnDefaultController();

        UWildBehaviorComponent* Behavior = NewObject<UWildBehaviorComponent>(Wild);
        Behavior->RegisterComponent();
        const float Maturity = FMath::FRandRange(Entry->MaturityMin, Entry->MaturityMax);
        Behavior->InitializeWild(State, Maturity);

        // A Blight-state wild one arrives already wounded by its ground.
        // 55 lands in the Corrupted band: its Corruption form manifests,
        // and purification becomes part of the meeting.
        if (State == EEncounterState::Blight)
        {
            if (UCorruptionSubsystem* Corruption = GetWorld()->GetSubsystem<UCorruptionSubsystem>())
            {
                Corruption->AccumulateCorruption(Entry->EcoKinID, 55.0f, ECorruptionSource::BlightZone);
            }
        }

        LivingWild.Add(Wild);
    }
    return true;
}

void AWildEcoKinSpawner::PruneDistant()
{
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    const float MaxDistSq = FMath::Square(SpawnTable.DespawnDistance);

    for (int32 i = LivingWild.Num() - 1; i >= 0; --i)
    {
        AEcoKinCompanion* Wild = LivingWild[i].Get();
        if (!Wild)
        {
            LivingWild.RemoveAt(i);
            continue;
        }
        if (Player && FVector::DistSquared(Wild->GetActorLocation(), Player->GetActorLocation()) > MaxDistSq)
        {
            // Released back to the wild, not destroyed as a failure — it was
            // never yours. The behavior component records nothing; there was
            // no meeting.
            Wild->Destroy();
            LivingWild.RemoveAt(i);
        }
    }
}
