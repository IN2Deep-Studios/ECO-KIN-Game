// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Tamer/TamerTypes.h"
#include "FrequencyTamerCharacter.generated.h"

class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UAnimaLinkComponent;

// The player character: a Frequency Tamer (Core-Binder specialization at high
// attunement). Moves, explores, scans with the A.E.G.I.S. device, and fights
// THROUGH the bonded Eco-Kin — the tamer never deals damage directly. Enemy
// aggression targets the Eco-Kin; link feedback is what can hurt the tamer.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API AFrequencyTamerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFrequencyTamerCharacter();

    // Bidirectional link endpoint. Eco-Kin strain/damage arrives here via
    // UAnimaLinkCombatSubsystem and lands in Vitals.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echohearts|AnimaLink")
    UAnimaLinkComponent* AnimaLink;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Tamer")
    FTamerVitals Vitals;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AEGIS")
    FAegisDeviceState Aegis;

    // Applies link feedback: the bonded partner's pain becomes tamer stamina
    // loss, spilling into health when stamina is gone. Called only through
    // the Anima-Link combat path — never directly by enemy damage.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|AnimaLink")
    void ApplyAnimaFeedback(float StaminaCost);

    // A.E.G.I.S. scan: reveals Eco-Kin, resources, and resonance points nearby.
    // Costs device charge; refused when the charge is too low.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|AEGIS")
    bool StartScan();

    UFUNCTION(BlueprintCallable, Category = "Echohearts|AEGIS")
    void StopScan();

    // Commands the bonded Eco-Kin (attack/defend/follow). Each command pays
    // link strain through UAnimaLinkCombatSubsystem — refused when the link
    // cannot bear it or the tamer is exhausted.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Companion")
    bool CommandCompanion(FName CommandId);

    // Which Eco-Kin is currently bonded and deployed beside the tamer.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Companion")
    AActor* ActiveCompanion = nullptr;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Enhanced Input actions (bound in the player controller).
    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|Input")
    UInputAction* MoveAction;
    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|Input")
    UInputAction* LookAction;
    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|Input")
    UInputAction* JumpAction;
    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|Input")
    UInputAction* InteractAction;
    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|Input")
    UInputAction* ScanAction;
    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|Input")
    UInputAction* DodgeAction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echohearts|Camera")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echohearts|Camera")
    UCameraComponent* FollowCamera;

private:
    void OnMove(const struct FInputActionValue& Value);
    void OnLook(const struct FInputActionValue& Value);

    // Checks whether stamina hit zero and reports the collapse to the
    // Infirmary once per fall. The Anima-Link's price, made visible.
    void CheckCollapse(float StrainDebt);
    bool bCollapseReported = false;
    void OnDodge();

    float StaminaRegenPerSecond = 8.0f;
    float ScanChargePerSecond = 12.0f;
};
