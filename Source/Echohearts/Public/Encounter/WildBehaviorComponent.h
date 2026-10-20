// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Encounter/EncounterTypes.h"
#include "WildBehaviorComponent.generated.h"

class AEcoKinCompanion;
class AAIController;

// The mind of a WILD EcoKin — unbonded, with its own agenda. Added at runtime
// by the spawner; the bonded AI controller stands down on its own because a
// wild companion has no tamer and no combat target (its guards already check).
//
// State machine: wander and graze while Unaware; notice the tamer when close;
// then act by encounter state. Calm ones grow curious and drift closer. Rage
// and Blight ones charge. Alpha ones warn, then charge if pressed. Hollow
// ones flee — they are hurt, not hostile.
//
// A wild EcoKin is never killed: at zero health it withdraws and flees.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-17).
UCLASS(ClassGroup = (Echohearts), meta = (BlueprintSpawnableComponent))
class ECHOHEARTS_API UWildBehaviorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWildBehaviorComponent();

    // The encounter state this wild one spawned in. Fixed at spawn.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Encounter")
    EEncounterState EncounterState = EEncounterState::Calm;

    // 0 (juvenile) to 1 (elder). Set at spawn; scales size and attributes.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Encounter")
    float WildMaturity = 0.5f;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Encounter")
    EEncounterPhase CurrentPhase = EEncounterPhase::Unaware;

    // Called by the spawner immediately after spawn. Applies state + maturity.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Encounter")
    void InitializeWild(EEncounterState InState, float InMaturity);

    // The tamer offers Binding (Resonance Alignment). The wild one stops,
    // listens, and yields to the ceremony system. An offer is a question —
    // a companion is a relationship, not a container.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Encounter")
    void OfferBinding();

    // Calm a hostile wild one without Binding — the "leave it be" path.
    // A soothed Hollow one that accepts care resolves as Befriended.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Encounter")
    void Soothe(AActor* Tamer);

    // Sent fleeing: after a lost duel, a refused Binding, or a frightened
    // Hollow. Runs from the threat, then releases back to the wild.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Encounter")
    void FleeFrom(AActor* Threat);

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    AEcoKinCompanion* GetWildCompanion() const;
    AAIController* GetWildController() const;
    AActor* FindNearestTamer() const;
    bool IsHostileState() const;

    void UpdateUnaware(float DeltaTime, AActor* Tamer, float DistToTamer);
    void UpdateAware(float DeltaTime, AActor* Tamer, float DistToTamer);
    void UpdateEngaged(float DeltaTime, AActor* Tamer, float DistToTamer);
    void UpdateFleeing(float DeltaTime);
    void WanderStep();
    void FaceTamer(AActor* Tamer);
    void ApplyStateModifiers();
    void ReleaseToWild(EEncounterResolution Resolution);

    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|Encounter")
    float NoticeRadius = 1500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|Encounter")
    float CuriosityRadius = 700.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|Encounter")
    float EngageRadius = 350.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|Encounter")
    float WanderRadius = 1200.0f;

    FVector HomeLocation = FVector::ZeroVector;
    bool bFleeing = false;
    AActor* FleeThreat = nullptr;
    float FleeTimer = 0.0f;
    bool bEncounterAnnounced = false;
};
