// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EcoKin/EcoKinCoreAttributes.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "EcoKinCompanion.generated.h"

class UAnimaLinkComponent;

// A bonded Eco-Kin present in the world beside its tamer. This is a PARTNER,
// not a pet and not a projectile: it has its own will (see AI controller),
// its own health, and its own judgment — a command is a request routed through
// the Anima-Link, and a strained or distrustful partner may hesitate.
// A companion is a relationship, not a container.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API AEcoKinCompanion : public ACharacter
{
    GENERATED_BODY()

public:
    AEcoKinCompanion();

    // Permanent Dex identity. Never changes; forms attach to it.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Identity")
    int32 EcoKinID = 0;

    // Locked four-attribute matrix. Never ATK/HP/SPD/DEF.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Attributes")
    FEcoKinCoreAttributes Attributes;

    // Current Ecoform (Base, GrowthRite, Mutation, ResonanceStrain, Shimmer...).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Identity")
    EEcoformKind CurrentForm = EEcoformKind::Base;

    // Essences this Eco-Kin can wield.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Essence")
    TArray<EEcoKinEssence> Essences;

    // 0-100. Damage taken in combat. At 0 the Eco-Kin withdraws (never dies —
    // partners are never killed; they retreat to recover).
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Combat")
    float Health = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echohearts|AnimaLink")
    UAnimaLinkComponent* AnimaLink;

    // Takes combat damage through the Anima-Link combat subsystem so feedback
    // reaches the tamer. Returns the stamina feedback the tamer owes.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Combat")
    float TakeLinkDamage(float DamageAmount);

    // Withdraws from combat to recover. Not death — retreat with dignity.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Combat")
    void Withdraw();

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Combat")
    bool bWithdrawn = false;

    // The tamer this companion is bonded to.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Bond")
    AActor* BondedTamer = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Bond")
    void SetBondedTamer(AActor* NewTamer);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    void RecoverTick(float DeltaSeconds);
};
