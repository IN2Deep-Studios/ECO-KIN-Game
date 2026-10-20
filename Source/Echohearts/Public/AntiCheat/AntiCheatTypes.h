// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "AntiCheatTypes.generated.h"

// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-14).
// Game-side anti-cheat interface. Server-authoritative: the client ATTESTS,
// the server VALIDATES. Client-supplied integrity claims are never trusted.
// Linux-safe: userspace only, no kernel-driver assumptions. Kernel-level
// protection on Linux arrives via Epic's EAC Linux v2 SDK (not yet released
// to developers); this interface is where its session callbacks will hook in.

// Result of a session attestation check.
UENUM(BlueprintType)
enum class EAttestationResult : uint8
{
    // Server validated the session.
    Trusted     UMETA(DisplayName = "Trusted"),
    // Validation inconclusive; restrict sensitive actions, keep playing.
    Unverified  UMETA(DisplayName = "Unverified"),
    // Server rejected the session; refuse online features.
    Rejected    UMETA(DisplayName = "Rejected")
};

// What the client attests about its own session. All fields are CLAIMS;
// the server re-verifies everything that matters.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FSessionAttestation
{
    GENERATED_BODY()

    // Platform the client reports: "Windows", "Mac", "Linux".
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AntiCheat")
    FString ReportedPlatform;

    // EAC module state, when an EAC SDK is linked. Empty when none.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AntiCheat")
    FString EacModuleState;

    // Opaque client token; server validates, never trusts.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AntiCheat")
    FString ClientToken;
};

// Server-side verdict on one attestation.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FAttestationVerdict
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AntiCheat")
    EAttestationResult Result = EAttestationResult::Unverified;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|AntiCheat")
    FText Reason;
};
