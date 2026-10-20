// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Combat/TargetingComponent.h"
#include "Companion/EcoKinCompanion.h"
#include "Tamer/FrequencyTamerCharacter.h"
#include "Game/EchoheartsGameMode.h"
#include "Kismet/GameplayStatics.h"

UTargetingComponent::UTargetingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UTargetingComponent::BeginPlay()
{
    Super::BeginPlay();
}

bool UTargetingComponent::AcquireTarget(float Range)
{
    DropTarget();

    // Canon guard: rites and ceremonies are never interrupted by targeting.
    if (AGameModeBase* GM = UGameplayStatics::GetGameMode(this))
    {
        if (AEchoheartsGameMode* EGM = Cast<AEchoheartsGameMode>(GM))
        {
            if (EGM->CurrentPhase == EGamePhase::Ceremony)
            {
                return false;
            }
        }
    }

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return false;
    }

    AEcoKinCompanion* SelfCompanion = nullptr;
    if (AFrequencyTamerCharacter* Tamer = Cast<AFrequencyTamerCharacter>(Owner))
    {
        SelfCompanion = Cast<AEcoKinCompanion>(Tamer->ActiveCompanion);
    }

    AActor* Best = nullptr;
    float BestScore = -1.0f;

    TArray<AActor*> Candidates;
    UGameplayStatics::GetAllActorsOfClass(this, AEcoKinCompanion::StaticClass(), Candidates);
    for (AActor* Candidate : Candidates)
    {
        if (!IsValidTarget(Candidate, SelfCompanion))
        {
            continue;
        }
        const float Dist = FVector::Dist(Owner->GetActorLocation(), Candidate->GetActorLocation());
        if (Dist > Range)
        {
            continue;
        }
        // Prefer closer, healthier threats.
        const float Score = (1.0f - Dist / Range) * 2.0f;
        if (Score > BestScore)
        {
            BestScore = Score;
            Best = Candidate;
        }
    }

    CurrentTarget = Best;
    return CurrentTarget != nullptr;
}

void UTargetingComponent::DropTarget()
{
    CurrentTarget = nullptr;
}

bool UTargetingComponent::IsValidTarget(AActor* Candidate, AActor* SelfCompanion) const
{
    if (!Candidate || Candidate == SelfCompanion)
    {
        return false;
    }
    AEcoKinCompanion* Companion = Cast<AEcoKinCompanion>(Candidate);
    if (!Companion || Companion->bWithdrawn)
    {
        return false;
    }
    // Never target the tamer's own bonded partner.
    AActor* Owner = GetOwner();
    if (AFrequencyTamerCharacter* Tamer = Cast<AFrequencyTamerCharacter>(Owner))
    {
        if (Companion->BondedTamer == Tamer)
        {
            return false;
        }
    }
    return true;
}
