// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "EcoKinTaxonomy.generated.h"

// Locked canon: the nine operational Essences of Rebearth.
// Printed legacy element labels (Hydro, Magma, Volt, Spirit, Solar, Iron, Chaos, ...)
// are migration terms only and must NOT be added here.
UENUM(BlueprintType)
enum class EEcoKinEssence : uint8
{
    Flora   UMETA(DisplayName = "Flora"),
    Torrent UMETA(DisplayName = "Torrent"),
    Pyre    UMETA(DisplayName = "Pyre"),
    Terra   UMETA(DisplayName = "Terra"),
    Aero    UMETA(DisplayName = "Aero"),
    Glaze   UMETA(DisplayName = "Glaze"),
    Voltic  UMETA(DisplayName = "Voltic"),
    Aura    UMETA(DisplayName = "Aura"),
    Shade   UMETA(DisplayName = "Shade")
};

// Locked canon: the five Orders of Sentience. EcoKin = the creature itself.
UENUM(BlueprintType)
enum class EEcoKinOrder : uint8
{
    Zoa    UMETA(DisplayName = "Zoa"),
    Flora  UMETA(DisplayName = "Flora"),
    Myko   UMETA(DisplayName = "Myko"),
    Proto  UMETA(DisplayName = "Proto"),
    Monera UMETA(DisplayName = "Monera")
};

// Locked canon: Ecoform = a form/state an EcoKin takes on. A form is never a
// separate creature identity; the Forms Registry keeps forms attached to base EcoKin.
UENUM(BlueprintType)
enum class EEcoformKind : uint8
{
    Base            UMETA(DisplayName = "Base"),
    GrowthRite      UMETA(DisplayName = "Growth Rite"),
    Mutation        UMETA(DisplayName = "Mutation"),
    ResonanceStrain UMETA(DisplayName = "Resonance Strain"),
    ShimmerForm     UMETA(DisplayName = "Shimmer Form"),
    BiomimeticShift UMETA(DisplayName = "Biomimetic Shift"),
    ResonantMorph   UMETA(DisplayName = "Resonant Morph"),
    PrimalMorph     UMETA(DisplayName = "Primal Morph")
};

// Locked canon: Shadow is an Echoheart-fracture CONDITION, never an element or category.
// The primary corruption language is The Blight / Data Virus.
UENUM(BlueprintType)
enum class EEcoKinCondition : uint8
{
    Healthy         UMETA(DisplayName = "Healthy"),
    ShadowFractured UMETA(DisplayName = "Shadow-Fractured"),
    Blighted        UMETA(DisplayName = "Blighted")
};

// Locked canon bond loop: Observe -> Protect -> Calm -> Kindle -> Bond / Release / Defer.
// A companion is a relationship, not a container. No capture, no ownership-by-force.
UENUM(BlueprintType)
enum class EEcoKinBondState : uint8
{
    Unbound   UMETA(DisplayName = "Unbound"),
    Observing UMETA(DisplayName = "Observing"),
    Protected UMETA(DisplayName = "Protected"),
    Calmed    UMETA(DisplayName = "Calmed"),
    Kindled   UMETA(DisplayName = "Kindled"),
    Bonded    UMETA(DisplayName = "Bonded"),
    Released  UMETA(DisplayName = "Released"),
    Deferred  UMETA(DisplayName = "Deferred")
};

// Locked canon: every Eco-Kin has an immutable Echoprint (permanent Dex identity).
// An approved appearance is never silently replaced.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FEcoKinIdentity
{
    GENERATED_BODY()

    // Permanent Production Dex ID. IDs 001-125 are the protected core roster and
    // must never be displaced, renumbered, or overwritten by seasonal additions.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Identity")
    int32 EcoKinID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Identity")
    FName Name = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Identity")
    EEcoKinOrder Order = EEcoKinOrder::Zoa;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Identity")
    EEcoKinEssence PrimaryEssence = EEcoKinEssence::Flora;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Identity")
    EEcoKinEssence SecondaryEssence = EEcoKinEssence::Flora;

    // Hash of the approved canonical appearance. Immutable once registered.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Identity")
    FString EchoprintHash;
};

// A single Ecoform attached to its base EcoKin identity.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FEcoformRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ecoform")
    FName FormID = NAME_None;

    // The base EcoKin this form belongs to. Forms never stand alone.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ecoform")
    int32 BaseEcoKinID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ecoform")
    EEcoformKind Kind = EEcoformKind::Base;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ecoform")
    FName DisplayName = NAME_None;

    // Shimmer Form is the universal ascension and must preserve identity.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ecoform")
    bool bPreservesIdentity = true;

    // Mutation node IDs granted by this form (Resonance Strain / Mutation kinds).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Ecoform")
    TArray<FName> MutationNodeIDs;
};

// ECO-API-001: shared result-event envelope. Every runtime contract call returns
// FEcoResult instead of bare bools so failures carry a namespaced error code.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FEcoResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Result")
    bool bSucceeded = true;

    // Namespaced code, e.g. "EcoKin.Squad.OverCapacity". NAME_None when bSucceeded.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Result")
    FName ErrorCode = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Result")
    FText Message;

    static FEcoResult Ok();
    static FEcoResult Fail(FName InErrorCode, const FText& InMessage);
};

// Enforces the locked bond loop transitions. Free function so Blueprints,
// UI, and tests share one transition table.
inline bool IsValidBondTransition(EEcoKinBondState From, EEcoKinBondState To)
{
    switch (From)
    {
    case EEcoKinBondState::Unbound:
        return To == EEcoKinBondState::Observing;
    case EEcoKinBondState::Observing:
        return To == EEcoKinBondState::Protected
            || To == EEcoKinBondState::Released
            || To == EEcoKinBondState::Deferred;
    case EEcoKinBondState::Protected:
        return To == EEcoKinBondState::Calmed;
    case EEcoKinBondState::Calmed:
        return To == EEcoKinBondState::Kindled;
    case EEcoKinBondState::Kindled:
        return To == EEcoKinBondState::Bonded
            || To == EEcoKinBondState::Released
            || To == EEcoKinBondState::Deferred;
    case EEcoKinBondState::Bonded:
        return To == EEcoKinBondState::Released;
    case EEcoKinBondState::Released:
    case EEcoKinBondState::Deferred:
        // A released or deferred Eco-Kin may be observed again later.
        return To == EEcoKinBondState::Observing;
    default:
        return false;
    }
}
