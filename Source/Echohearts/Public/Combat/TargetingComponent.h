// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"

// Soft target acquisition for the tamer's camera and the companion's focus.
// Picks the most threatening linked combatant in front of the tamer; never
// targets the tamer's own bonded partner or non-combatants.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS(ClassGroup = (Echohearts), meta = (BlueprintSpawnableComponent))
class ECHOHEARTS_API UTargetingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTargetingComponent();

    // Current soft target (null when nothing qualifies).
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Targeting")
    AActor* CurrentTarget = nullptr;

    // Scans for the best target within Range of the owner. Refused during
    // Ceremony phase — rites are never interrupted by targeting.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Targeting")
    bool AcquireTarget(float Range = 2500.0f);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Targeting")
    void DropTarget();

protected:
    virtual void BeginPlay() override;

private:
    bool IsValidTarget(AActor* Candidate, AActor* SelfCompanion) const;
};
