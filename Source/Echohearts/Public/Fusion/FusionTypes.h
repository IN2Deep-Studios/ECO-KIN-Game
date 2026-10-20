// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "FusionTypes.generated.h"

// The three stages of a fused EcoKin's Growth Rite path. A combined being
// must prove its two halves can live as one before it is allowed to grow.
UENUM(BlueprintType)
enum class EFusionRiteStage : uint8
{
    // The parents' rhythms learn each other's tempo. Trust begins here.
    Attunement       UMETA(DisplayName = "Attunement"),
    // Two natures, one stride. The fusion acts as a single creature.
    Harmonization    UMETA(DisplayName = "Harmonization"),
    // The combined being stands on its own: a sovereign self, not a sum.
    SovereignConcord UMETA(DisplayName = "Sovereign Concord")
};

UENUM(BlueprintType)
enum class EFusionRefusalReason : uint8
{
    None                 UMETA(DisplayName = "None"),
    NotRegisteredPair    UMETA(DisplayName = "Not Registered Pair"),
    AlreadyFused         UMETA(DisplayName = "Already Fused"),
    CatalystMissing      UMETA(DisplayName = "Catalyst Missing"),
    UnstableLineage      UMETA(DisplayName = "Unstable Lineage"),
    ParentsNotDormant    UMETA(DisplayName = "Parents Not Dormant"),
    NoRecord             UMETA(DisplayName = "No Record")
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FFusionResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    bool bSucceeded = false;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    EFusionRefusalReason RefusalReason = EFusionRefusalReason::None;

    // The fused being's own Dex ID. A combined EcoKin is a stable identity
    // with its own lineage record — never a temporary state.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    int32 FusedDexID = 0;

    static FFusionResult Ok(int32 InFusedDexID)
    {
        FFusionResult R;
        R.bSucceeded = true;
        R.FusedDexID = InFusedDexID;
        return R;
    }

    static FFusionResult Refused(EFusionRefusalReason Reason)
    {
        FFusionResult R;
        R.bSucceeded = false;
        R.RefusalReason = Reason;
        return R;
    }
};

// The permanent record of one Concord Fusion. Parents are NEVER consumed:
// they enter dormant resonance inside the fusion, and unfusing restores
// them fully — same Echoprints, same trust, same selves.
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FFusionRecord
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    int32 ParentADexID = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    int32 ParentBDexID = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    int32 FusedDexID = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    FString FusedName;

    // Mutation nodes inherited from each parent, plus the one node that
    // exists only because THESE two became one.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    TArray<FName> InheritedNodesA;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    TArray<FName> InheritedNodesB;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    FName FusionUniqueNode = NAME_None;

    // Essence expressions carried from each parent (nine Essences only).
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    TArray<FName> InheritedEssences;

    // 0–100. Synth-Origin parentage lowers it; D.A.H.L.I.A. validation is
    // required before an unstable fusion may attempt Growth Rites.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    float DNAStability = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    bool bDahliaValidated = false;

    // True while the parents rest in dormant resonance. Unfusing flips this
    // back and both parents return exactly as they were.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    bool bParentsDormant = true;

    // Highest rite stage completed by this fusion.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    EFusionRiteStage CompletedRiteStage = EFusionRiteStage::Attunement;

    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    bool bHasCompletedAnyRite = false;

    // True when either parent carries Nature lineage: every mutation then
    // requires its explicit condition — no exceptions, no shortcuts.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Fusion")
    bool bNatureLineage = false;
};
