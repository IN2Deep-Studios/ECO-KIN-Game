// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Tamer/FrequencyTamerCharacter.h"
#include "AnimaLink/AnimaLinkComponent.h"
#include "Sanctuary/InfirmarySubsystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

AFrequencyTamerCharacter::AFrequencyTamerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    AnimaLink = CreateDefaultSubobject<UAnimaLinkComponent>(TEXT("AnimaLink"));

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 420.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AFrequencyTamerCharacter::BeginPlay()
{
    Super::BeginPlay();
    Vitals.Clamp();
    Aegis.Clamp();
}

void AFrequencyTamerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Stamina recovers at rest; never while the link is at fracture risk —
    // a strained bond keeps the tamer tense.
    const bool bLinkStrained = AnimaLink && AnimaLink->LinkState.IsFractureRisk();
    if (!bLinkStrained && Vitals.Stamina < 100.0f)
    {
        Vitals.Stamina = FMath::Min(100.0f, Vitals.Stamina + StaminaRegenPerSecond * DeltaSeconds);
    }

    if (Aegis.bScanning)
    {
        Aegis.Charge = FMath::Max(0.0f, Aegis.Charge - ScanChargePerSecond * DeltaSeconds);
        if (Aegis.Charge <= 0.0f)
        {
            StopScan();
        }
    }
}

void AFrequencyTamerCharacter::ApplyAnimaFeedback(float StaminaCost)
{
    // Canon: the partner's pain becomes the tamer's burden. Stamina absorbs it
    // first; overflow reaches health. This is the ONLY way combat reaches the
    // tamer — enemies cannot damage the tamer directly.
    Vitals.Stamina -= StaminaCost;
    if (Vitals.Stamina < 0.0f)
    {
        Vitals.Health += Vitals.Stamina * 0.5f; // overflow at half rate
        Vitals.Stamina = 0.0f;
    }
    Vitals.Clamp();
    CheckCollapse(StaminaCost);
}

bool AFrequencyTamerCharacter::StartScan()
{
    if (Aegis.bScanning || Aegis.Charge < 10.0f)
    {
        return false;
    }
    Aegis.bScanning = true;
    return true;
}

void AFrequencyTamerCharacter::StopScan()
{
    Aegis.bScanning = false;
}

bool AFrequencyTamerCharacter::CommandCompanion(FName CommandId)
{
    // Exhausted tamers cannot command; the link refuses what it cannot bear.
    // The strain payment itself happens in UAnimaLinkCombatSubsystem.
    if (Vitals.IsExhausted() || !ActiveCompanion)
    {
        return false;
    }
    Vitals.Stamina = FMath::Max(0.0f, Vitals.Stamina - 5.0f);
    return true;
}

void AFrequencyTamerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)     EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFrequencyTamerCharacter::OnMove);
        if (LookAction)     EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFrequencyTamerCharacter::OnLook);
        if (JumpAction)     EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        if (JumpAction)     EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        if (ScanAction)     EIC->BindAction(ScanAction, ETriggerEvent::Started, this, &AFrequencyTamerCharacter::StartScan);
        if (ScanAction)     EIC->BindAction(ScanAction, ETriggerEvent::Completed, this, &AFrequencyTamerCharacter::StopScan);
        if (DodgeAction)    EIC->BindAction(DodgeAction, ETriggerEvent::Started, this, &AFrequencyTamerCharacter::OnDodge);
        if (InteractAction) EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &AFrequencyTamerCharacter::CommandCompanion, FName(TEXT("Interact")));
    }
}

void AFrequencyTamerCharacter::OnMove(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (Controller && (Axis.X != 0.0f || Axis.Y != 0.0f))
    {
        const FRotator Yaw(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
        AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X), Axis.Y);
        AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), Axis.X);
    }
}

void AFrequencyTamerCharacter::OnLook(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (Controller)
    {
        AddControllerYawInput(Axis.X);
        AddControllerPitchInput(Axis.Y);
    }
}

void AFrequencyTamerCharacter::OnDodge()
{
    // A quick burst of movement. Costs stamina; impossible when exhausted.
    // Canon: dodging never breaks the Anima-Link — the bond moves with you.
    if (Vitals.Stamina < 15.0f)
    {
        return;
    }
    Vitals.Stamina -= 15.0f;
    LaunchCharacter(GetVelocity().GetSafeNormal() * 900.0f + FVector(0.0f, 0.0f, 200.0f), true, true);
}

void AFrequencyTamerCharacter::CheckCollapse(float StrainDebt)
{
    if (Vitals.Stamina > 0.0f)
    {
        bCollapseReported = false;
        return;
    }
    if (bCollapseReported)
    {
        return;
    }
    bCollapseReported = true;
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UInfirmarySubsystem* Infirmary = GI->GetSubsystem<UInfirmarySubsystem>())
        {
            Infirmary->CollapseTamer(StrainDebt);
        }
    }
}
