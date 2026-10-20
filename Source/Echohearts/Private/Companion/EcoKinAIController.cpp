// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Companion/EcoKinAIController.h"
#include "Companion/EcoKinCompanion.h"

AEcoKinAIController::AEcoKinAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEcoKinAIController::BeginPlay()
{
    Super::BeginPlay();
}

bool AEcoKinAIController::RequestStance(ECompanionStance NewStance, float TrustPercent, float LinkStrain)
{
    // Canon: a companion is a relationship. Low trust or a strained link
    // makes a partner hesitate — the command is heard, not puppeted.
    if (TrustPercent < 25.0f || LinkStrain > 85.0f)
    {
        CurrentStance = ECompanionStance::Hesitate;
        return false;
    }
    CurrentStance = NewStance;
    return true;
}

void AEcoKinAIController::SetCombatTarget(AActor* NewTarget)
{
    CombatTarget = NewTarget;
}

void AEcoKinAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    switch (CurrentStance)
    {
    case ECompanionStance::Follow:
    case ECompanionStance::Guard:
        UpdateFollow(DeltaSeconds);
        break;
    case ECompanionStance::Engage:
        UpdateEngage(DeltaSeconds);
        break;
    case ECompanionStance::Retreat:
        // Falls back toward the tamer; handled by UpdateFollow with distance.
        UpdateFollow(DeltaSeconds);
        break;
    case ECompanionStance::Hesitate:
        StopMovement();
        break;
    }
}

void AEcoKinAIController::UpdateFollow(float DeltaSeconds)
{
    AEcoKinCompanion* Companion = Cast<AEcoKinCompanion>(GetPawn());
    if (!Companion || !Companion->BondedTamer)
    {
        return;
    }

    const FVector ToTamer = Companion->BondedTamer->GetActorLocation() - Companion->GetActorLocation();
    if (ToTamer.Size() > FollowDistance)
    {
        MoveToActor(Companion->BondedTamer, FollowDistance * 0.7f);
    }
}

void AEcoKinAIController::UpdateEngage(float DeltaSeconds)
{
    if (!CombatTarget)
    {
        CurrentStance = ECompanionStance::Follow;
        return;
    }
    MoveToActor(CombatTarget, 150.0f);
}
