// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Encounter/WildBehaviorComponent.h"
#include "Companion/EcoKinCompanion.h"
#include "Encounter/EncounterSubsystem.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

UWildBehaviorComponent::UWildBehaviorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWildBehaviorComponent::BeginPlay()
{
    Super::BeginPlay();
    HomeLocation = GetOwner()->GetActorLocation();
}

void UWildBehaviorComponent::InitializeWild(EEncounterState InState, float InMaturity)
{
    EncounterState = InState;
    WildMaturity = FMath::Clamp(InMaturity, 0.0f, 1.0f);
    ApplyStateModifiers();
}

void UWildBehaviorComponent::ApplyStateModifiers()
{
    AEcoKinCompanion* Companion = GetWildCompanion();
    if (!Companion)
    {
        return;
    }

    // Maturity scales the body and the four attributes. Never a "level."
    const float MaturityScale = 0.7f + 0.6f * WildMaturity;
    Companion->Attributes.Vibrance *= MaturityScale;
    Companion->Attributes.Density *= MaturityScale;
    Companion->Attributes.Harmony *= MaturityScale;
    Companion->Attributes.Purity *= MaturityScale;

    float BodyScale = 0.8f + 0.5f * WildMaturity;

    switch (EncounterState)
    {
    case EEncounterState::Alpha:
        Companion->Attributes.Vibrance *= 1.25f;
        Companion->Attributes.Density *= 1.25f;
        BodyScale *= 1.25f;
        break;
    case EEncounterState::Rage:
        Companion->Attributes.Vibrance *= 1.15f;
        Companion->Attributes.Density *= 1.15f;
        break;
    case EEncounterState::Hollow:
        Companion->Attributes.Vibrance *= 0.7f;
        Companion->Attributes.Density *= 0.7f;
        Companion->Attributes.Harmony *= 0.7f;
        Companion->Attributes.Purity *= 0.7f;
        break;
    case EEncounterState::Blight:
        // Corruption itself is applied by the spawner through the Corruption
        // subsystem (AccumulateCorruption). The body shows it; stats stay.
        break;
    case EEncounterState::Calm:
    default:
        break;
    }

    Companion->Attributes.Clamp();
    Companion->SetActorScale3D(FVector(BodyScale));
}

AEcoKinCompanion* UWildBehaviorComponent::GetWildCompanion() const
{
    return Cast<AEcoKinCompanion>(GetOwner());
}

AAIController* UWildBehaviorComponent::GetWildController() const
{
    AEcoKinCompanion* Companion = GetWildCompanion();
    return Companion ? Cast<AAIController>(Companion->GetController()) : nullptr;
}

AActor* UWildBehaviorComponent::FindNearestTamer() const
{
    // Single-player: the player character is the tamer. Multi-tamer support
    // arrives with the session layer.
    return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

bool UWildBehaviorComponent::IsHostileState() const
{
    return EncounterState == EEncounterState::Rage
        || EncounterState == EEncounterState::Blight
        || EncounterState == EEncounterState::Alpha;
}

void UWildBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AEcoKinCompanion* Companion = GetWildCompanion();
    if (!IsValid(Companion))
    {
        return;
    }

    // Beaten in a duel: withdraw already fired inside TakeLinkDamage.
    // A wild one never dies — it runs.
    if (Companion->bWithdrawn && !bFleeing)
    {
        FleeFrom(FindNearestTamer());
        return;
    }

    if (bFleeing)
    {
        UpdateFleeing(DeltaTime);
        return;
    }

    AActor* Tamer = FindNearestTamer();
    if (!Tamer)
    {
        UpdateUnaware(DeltaTime, nullptr, FLT_MAX);
        return;
    }

    const float DistToTamer = FVector::Dist(Companion->GetActorLocation(), Tamer->GetActorLocation());

    switch (CurrentPhase)
    {
    case EEncounterPhase::Unaware:
        UpdateUnaware(DeltaTime, Tamer, DistToTamer);
        break;
    case EEncounterPhase::Aware:
        UpdateAware(DeltaTime, Tamer, DistToTamer);
        break;
    case EEncounterPhase::Engaged:
        UpdateEngaged(DeltaTime, Tamer, DistToTamer);
        break;
    case EEncounterPhase::BindingOffered:
        // Holding still, listening. If the tamer walks away, the moment passes.
        if (DistToTamer > NoticeRadius * 1.5f)
        {
            CurrentPhase = EEncounterPhase::Aware;
        }
        break;
    case EEncounterPhase::Resolved:
        break;
    }
}

void UWildBehaviorComponent::UpdateUnaware(float, AActor* Tamer, float DistToTamer)
{
    if (Tamer && DistToTamer < NoticeRadius)
    {
        // Hollow ones notice and immediately fear.
        if (EncounterState == EEncounterState::Hollow)
        {
            FleeFrom(Tamer);
            return;
        }
        CurrentPhase = EEncounterPhase::Aware;
        return;
    }
    WanderStep();
}

void UWildBehaviorComponent::UpdateAware(float DeltaTime, AActor* Tamer, float DistToTamer)
{
    if (!Tamer || DistToTamer > NoticeRadius * 1.5f)
    {
        CurrentPhase = EEncounterPhase::Unaware;
        return;
    }

    AAIController* AI = GetWildController();

    if (IsHostileState())
    {
        // Alpha warns first: holds ground and faces the tamer. Press closer
        // than curiosity range and the warning ends.
        if (EncounterState == EEncounterState::Alpha && DistToTamer > CuriosityRadius)
        {
            FaceTamer(Tamer);
            if (AI)
            {
                AI->StopMovement();
            }
            return;
        }
        // Rage and Blight charge; a pressed Alpha joins them.
        if (DistToTamer < EngageRadius && AI)
        {
            CurrentPhase = EEncounterPhase::Engaged;
        }
        else if (AI)
        {
            AI->MoveToActor(Tamer, EngageRadius * 0.8f);
        }
        return;
    }

    // Calm: curiosity. Drift closer, unhurried.
    if (DistToTamer < CuriosityRadius && AI)
    {
        AI->MoveToActor(Tamer, EngageRadius);
    }
    if (DistToTamer < EngageRadius)
    {
        CurrentPhase = EEncounterPhase::Engaged;
    }
}

void UWildBehaviorComponent::UpdateEngaged(float DeltaTime, AActor* Tamer, float DistToTamer)
{
    if (!bEncounterAnnounced)
    {
        bEncounterAnnounced = true;
        if (UWorld* World = GetWorld())
        {
            if (UEncounterSubsystem* Encounters = World->GetSubsystem<UEncounterSubsystem>())
            {
                Encounters->BeginEncounter(GetWildCompanion(), Tamer);
            }
        }
    }

    FaceTamer(Tamer);

    // v1: hostile wild ones hold at range and threaten. Real fights resolve
    // through the Duel battle mode — this component never deals damage.
    if (IsHostileState() && DistToTamer < EngageRadius * 0.6f)
    {
        if (AAIController* AI = GetWildController())
        {
            const FVector Away = (GetOwner()->GetActorLocation() - Tamer->GetActorLocation()).GetSafeNormal();
            AI->MoveToLocation(GetOwner()->GetActorLocation() + Away * 200.0f, 50.0f);
        }
    }
}

void UWildBehaviorComponent::UpdateFleeing(float DeltaTime)
{
    FleeTimer -= DeltaTime;
    AEcoKinCompanion* Companion = GetWildCompanion();
    AActor* Tamer = FindNearestTamer();

    if (FleeTimer <= 0.0f || (Tamer && FVector::Dist(Companion->GetActorLocation(), Tamer->GetActorLocation()) > 4000.0f))
    {
        ReleaseToWild(EEncounterResolution::Fled);
        return;
    }

    if (AAIController* AI = GetWildController())
    {
        FVector Away = (HomeLocation - Companion->GetActorLocation()).GetSafeNormal();
        if (FleeThreat)
        {
            Away = (Companion->GetActorLocation() - FleeThreat->GetActorLocation()).GetSafeNormal();
        }
        AI->MoveToLocation(Companion->GetActorLocation() + Away * 1500.0f, 100.0f);
    }
}

void UWildBehaviorComponent::WanderStep()
{
    AAIController* AI = GetWildController();
    if (!AI || AI->GetMoveStatus() == EPathFollowingStatus::Moving)
    {
        return;
    }

    FVector Target = HomeLocation;
    if (UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld()))
    {
        FNavLocation OutLocation;
        if (Nav->GetRandomReachablePointInRadius(HomeLocation, WanderRadius, OutLocation))
        {
            Target = OutLocation.Location;
        }
    }
    else
    {
        Target = HomeLocation + FVector(FMath::FRandRange(-WanderRadius, WanderRadius),
                                       FMath::FRandRange(-WanderRadius, WanderRadius), 0.0f);
    }
    AI->MoveToLocation(Target, 120.0f);
}

void UWildBehaviorComponent::FaceTamer(AActor* Tamer)
{
    if (!Tamer)
    {
        return;
    }
    const FVector ToTamer = Tamer->GetActorLocation() - GetOwner()->GetActorLocation();
    if (!ToTamer.IsNearlyZero())
    {
        GetOwner()->SetActorRotation(ToTamer.Rotation());
    }
}

void UWildBehaviorComponent::OfferBinding()
{
    AEcoKinCompanion* Companion = GetWildCompanion();
    if (!Companion || CurrentPhase == EEncounterPhase::Resolved)
    {
        return;
    }
    CurrentPhase = EEncounterPhase::BindingOffered;
    if (AAIController* AI = GetWildController())
    {
        AI->StopMovement();
    }
    FaceTamer(FindNearestTamer());
    if (UWorld* World = GetWorld())
    {
        if (UEncounterSubsystem* Encounters = World->GetSubsystem<UEncounterSubsystem>())
        {
            Encounters->OfferBinding(Companion, FindNearestTamer());
        }
    }
}

void UWildBehaviorComponent::Soothe(AActor*)
{
    AEcoKinCompanion* Companion = GetWildCompanion();
    if (!Companion || CurrentPhase == EEncounterPhase::Resolved)
    {
        return;
    }

    if (EncounterState == EEncounterState::Hollow)
    {
        // It was never hostile — only hurt. Care accepted; it stays wild,
        // and the meeting is recorded as warmth, not ownership.
        ReleaseToWild(EEncounterResolution::Befriended);
        return;
    }

    if (IsHostileState())
    {
        // Talked down. It backs off, unharmed, dignity intact.
        if (AAIController* AI = GetWildController())
        {
            AI->StopMovement();
        }
        ReleaseToWild(EEncounterResolution::Repelled);
    }
}

void UWildBehaviorComponent::FleeFrom(AActor* Threat)
{
    if (bFleeing || CurrentPhase == EEncounterPhase::Resolved)
    {
        return;
    }
    bFleeing = true;
    FleeThreat = Threat ? Threat : FindNearestTamer();
    FleeTimer = 12.0f;
    CurrentPhase = EEncounterPhase::Resolved;
}

void UWildBehaviorComponent::ReleaseToWild(EEncounterResolution Resolution)
{
    AEcoKinCompanion* Companion = GetWildCompanion();
    CurrentPhase = EEncounterPhase::Resolved;
    if (UWorld* World = GetWorld())
    {
        if (UEncounterSubsystem* Encounters = World->GetSubsystem<UEncounterSubsystem>())
        {
            Encounters->EndEncounter(Companion, Resolution);
        }
    }
    if (Companion)
    {
        Companion->Destroy();
    }
}
