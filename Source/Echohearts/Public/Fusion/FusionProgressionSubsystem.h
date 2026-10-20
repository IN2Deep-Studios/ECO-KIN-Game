// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Fusion/FusionTypes.h"
#include "FusionProgressionSubsystem.generated.h"

// CONCORD FUSION — two become one, and both survive it.
//
// Concord Fusion is allowed ONLY for registered pairs. The hard rule, kept
// here in code: fusion NEVER kills or consumes the parents. Both parents
// enter dormant resonance inside the combined being; unfusing restores them
// fully — same Echoprints, same trust, same selves. The fused EcoKin is a
// stable combined identity with its own Dex ID and lineage record.
//
// Three paths grow a combined EcoKin:
// - GROWTH RITES: Attunement → Harmonization → Sovereign Concord. Each stage
//   requires BOTH parents' Biological Maturity, the fusion's own Kindling
//   (trust %), Ecology accomplishments, and a Concord Catalyst.
// - MUTATION: the fusion inherits mutation nodes from each parent plus one
//   unique fusion node. All triggers conditional, never random. Nature-
//   lineage fusions demand every condition, every time.
// - DNA: lineage record, inherited essence expressions, DNA stability score.
//   Synth-Origin parentage raises instability; D.A.H.L.I.A. must validate an
//   unstable fusion before it may attempt Growth Rites.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-16).
UCLASS()
class ECHOHEARTS_API UFusionProgressionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Registers a pair allowed to attempt Concord Fusion. Unregistered
    // pairs are refused — Concord is a commitment, not an experiment.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Fusion")
    bool RegisterConcordPair(int32 ParentADexID, int32 ParentBDexID);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Fusion")
    bool IsRegisteredPair(int32 ParentADexID, int32 ParentBDexID) const;

    // Attempts the fusion. Parents must both be biologically mature; a
    // Concord Catalyst is consumed. Returns the fused being's Dex ID.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Fusion")
    FFusionResult RequestFusion(int32 ParentADexID, int32 ParentBDexID,
        const FString& ParentAName, const FString& ParentBName,
        const FString& FusedName, bool bParentAMature, bool bParentBMature,
        bool bHasConcordCatalyst, bool bParentASynthOrigin, bool bParentBSynthOrigin,
        bool bNatureLineage, const TArray<FName>& NodesA, const TArray<FName>& NodesB,
        FName FusionUniqueNode, const TArray<FName>& InheritedEssences);

    // Unfusing restores both parents fully. The fused identity's record is
    // archived, never deleted — provenance is permanent.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Fusion")
    FFusionResult RequestUnfuse(int32 FusedDexID);

    // Growth Rite for a fused EcoKin. Gates per stage:
    // Attunement: fusion trust >= 40, 1 ecology accomplishment, catalyst.
    // Harmonization: fusion trust >= 65, 3 accomplishments, catalyst.
    // Sovereign Concord: fusion trust >= 85, 5 accomplishments, catalyst,
    //   D.A.H.L.I.A. validation if DNA stability < 60.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Fusion")
    FFusionResult RequestFusionRite(int32 FusedDexID, EFusionRiteStage Stage,
        float FusionTrustPercent, int32 EcologyAccomplishments, bool bHasCatalyst);

    // D.A.H.L.I.A. validates an unstable lineage. Required before Growth
    // Rites when DNA stability is below 60.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Fusion")
    bool ValidateWithDahlia(int32 FusedDexID);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Fusion")
    bool TryGetFusionRecord(int32 FusedDexID, FFusionRecord& OutRecord) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Fusion")
    bool IsFused(int32 FusedDexID) const;

    UFUNCTION(BlueprintPure, Category = "Echohearts|Fusion")
    float GetDNAStability(int32 FusedDexID) const;

private:
    int32 AllocateFusedDexID();
    float ComputeDNAStability(bool bParentASynthOrigin, bool bParentBSynthOrigin) const;

    static int64 PairKey(int32 A, int32 B)
    {
        const int32 Lo = FMath::Min(A, B);
        const int32 Hi = FMath::Max(A, B);
        return (static_cast<int64>(Lo) << 32) | static_cast<uint32>(Hi);
    }

    // Registered Concord pairs.
    UPROPERTY()
    TSet<int64> ConcordPairs;

    // Active fusion records, keyed by fused Dex ID.
    UPROPERTY()
    TMap<int32, FFusionRecord> FusionRecords;

    // Archived records of unfused beings (provenance preserved).
    UPROPERTY()
    TMap<int32, FFusionRecord> FusionArchive;

    // Fused Dex IDs are allocated above the wild Dex range.
    int32 NextFusedDexID = 100001;

    float StabilityThresholdForRites = 60.0f;
    float SynthOriginStabilityPenalty = 25.0f;
};
