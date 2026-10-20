// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EcoKinAIController.generated.h"

// What the companion is doing right now. Commands from the tamer are requests:
// a partner with low trust or a strained link may hesitate before obeying.
UENUM(BlueprintType)
enum class ECompanionStance : uint8
{
    Follow   UMETA(DisplayName = "Follow"),
    Guard    UMETA(DisplayName = "Guard"),
    Engage   UMETA(DisplayName = "Engage"),
    Retreat  UMETA(DisplayName = "Retreat"),
    Hesitate UMETA(DisplayName = "Hesitate")
};

// The companion's own mind. It follows, guards, engages, and retreats on its
// own judgment; tamer commands shift its stance but never puppet it.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API AEcoKinAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEcoKinAIController();

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|AI")
    ECompanionStance CurrentStance = ECompanionStance::Follow;

    // Requests a stance change. Returns false when the partner hesitates
    // (low trust or strained link) — the request is heard, not obeyed.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|AI")
    bool RequestStance(ECompanionStance NewStance, float TrustPercent, float LinkStrain);

    // Current combat target, if any.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|AI")
    AActor* CombatTarget = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Echohearts|AI")
    void SetCombatTarget(AActor* NewTarget);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    void UpdateFollow(float DeltaSeconds);
    void UpdateEngage(float DeltaSeconds);

    // How close the companion stays to its tamer while following.
    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|AI")
    float FollowDistance = 220.0f;
};
