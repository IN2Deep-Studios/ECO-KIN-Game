// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Companion/EcoKinCompanion.h"
#include "AnimaLink/AnimaLinkComponent.h"
#include "AnimaLink/AnimaLinkCombatSubsystem.h"
#include "Engine/GameInstance.h"

AEcoKinCompanion::AEcoKinCompanion()
{
    PrimaryActorTick.bCanEverTick = true;
    AnimaLink = CreateDefaultSubobject<UAnimaLinkComponent>(TEXT("AnimaLink"));
}

void AEcoKinCompanion::BeginPlay()
{
    Super::BeginPlay();
    Attributes.Clamp();
}

void AEcoKinCompanion::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    RecoverTick(DeltaSeconds);
}

float AEcoKinCompanion::TakeLinkDamage(float DamageAmount)
{
    if (bWithdrawn || DamageAmount <= 0.0f)
    {
        return 0.0f;
    }

    Health = FMath::Max(0.0f, Health - DamageAmount);

    // Route through the combat subsystem: feedback reaches the tamer, and a
    // fracture-risk exchange is refused rather than silently breaking.
    float TamerFeedback = 0.0f;
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UAnimaLinkCombatSubsystem* Combat = GI->GetSubsystem<UAnimaLinkCombatSubsystem>())
        {
            const FEcoResult Result = Combat->ResolveEcoKinHit(AnimaLink, DamageAmount, TamerFeedback);
            if (!Result.bSucceeded)
            {
                // Exchange refused (fracture risk): the hit is absorbed by the
                // bond's refusal, not by the partner's body.
                Health = FMath::Min(100.0f, Health + DamageAmount);
                return 0.0f;
            }
        }
    }

    if (Health <= 0.0f)
    {
        Withdraw();
    }
    return TamerFeedback;
}

void AEcoKinCompanion::Withdraw()
{
    // Partners are never killed. They withdraw, recover, and can return.
    bWithdrawn = true;
}

void AEcoKinCompanion::SetBondedTamer(AActor* NewTamer)
{
    BondedTamer = NewTamer;
}

void AEcoKinCompanion::RecoverTick(float DeltaSeconds)
{
    if (bWithdrawn && Health < 100.0f)
    {
        Health = FMath::Min(100.0f, Health + 5.0f * DeltaSeconds);
        if (Health >= 100.0f)
        {
            bWithdrawn = false;
        }
    }
}
