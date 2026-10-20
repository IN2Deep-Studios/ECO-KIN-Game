// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "EcoKin/EcoKinTaxonomy.h"
#include "MutationTypes.generated.h"

// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-14).
// Mutations produce actual Mutation FORMS (EEcoformKind::Mutation) attached to
// the base EcoKin identity — never separate creature identities. Tay's canon:
// mutations are conditional and meaningful, never random power creep.

// What can trigger a mutation.
UENUM(BlueprintType)
enum class EMutationTriggerKind : uint8
{
    // A Resonance Strain redistributes the stat budget and opens mutation nodes.
    ResonanceStrain      UMETA(DisplayName = "Resonance Strain"),
    // Shadestone attunement: Shade ATTUNEMENT, never corruption (canon lock).
    ShadestoneAttunement UMETA(DisplayName = "Shadestone Attunement"),
    // World-level happenings: aurora convergence, bloom tides, eclipse, etc.
    EcologicalEvent      UMETA(DisplayName = "Ecological Event")
};

// One mutation request against a registered EcoKin identity.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FMutationTrigger
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mutation")
    EMutationTriggerKind TriggerKind = EMutationTriggerKind::ResonanceStrain;

    // Base EcoKin Dex ID the mutation form will attach to.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mutation")
    int32 TargetEcoKinID = 0;

    // Catalyst item or event ID that provoked the mutation (e.g. a Shadestone,
    // a Resonance Shard data asset, or an ecological event key).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mutation")
    FName CatalystId = NAME_None;

    // Canon lock: Nature (Legendary Humanoid-Kin) allows CONDITIONAL Mutations
    // only. Unconditional mutation requests against Nature are refused.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mutation")
    bool bConditionalOnly = false;

    // Mutation node IDs granted by the resulting form.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mutation")
    TArray<FName> MutationNodeIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Mutation")
    FText MutationBlurb;
};
