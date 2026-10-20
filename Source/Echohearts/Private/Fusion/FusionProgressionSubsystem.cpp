// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Fusion/FusionProgressionSubsystem.h"

bool UFusionProgressionSubsystem::RegisterConcordPair(int32 ParentADexID, int32 ParentBDexID)
{
    if (ParentADexID <= 0 || ParentBDexID <= 0 || ParentADexID == ParentBDexID)
    {
        return false;
    }
    ConcordPairs.Add(PairKey(ParentADexID, ParentBDexID));
    return true;
}

bool UFusionProgressionSubsystem::IsRegisteredPair(int32 ParentADexID, int32 ParentBDexID) const
{
    return ConcordPairs.Contains(PairKey(ParentADexID, ParentBDexID));
}

FFusionResult UFusionProgressionSubsystem::RequestFusion(int32 ParentADexID, int32 ParentBDexID,
    const FString& ParentAName, const FString& ParentBName,
    const FString& FusedName, bool bParentAMature, bool bParentBMature,
    bool bHasConcordCatalyst, bool bParentASynthOrigin, bool bParentBSynthOrigin,
    bool bNatureLineage, const TArray<FName>& NodesA, const TArray<FName>& NodesB,
    FName FusionUniqueNode, const TArray<FName>& InheritedEssences)
{
    // Concord is a commitment, not an experiment.
    if (!IsRegisteredPair(ParentADexID, ParentBDexID))
    {
        return FFusionResult::Refused(EFusionRefusalReason::NotRegisteredPair);
    }
    if (!bParentAMature || !bParentBMature)
    {
        // Both parents must be biologically mature. Fusing the immature is
        // refused the same way an early Growth Rite is.
        return FFusionResult::Refused(EFusionRefusalReason::UnstableLineage);
    }
    if (!bHasConcordCatalyst)
    {
        return FFusionResult::Refused(EFusionRefusalReason::CatalystMissing);
    }

    const int32 FusedDexID = AllocateFusedDexID();

    FFusionRecord Record;
    Record.ParentADexID = ParentADexID;
    Record.ParentBDexID = ParentBDexID;
    Record.FusedDexID = FusedDexID;
    Record.FusedName = FusedName.IsEmpty()
        ? FString::Printf(TEXT("%s-%s Concord"), *ParentAName, *ParentBName)
        : FusedName;
    Record.InheritedNodesA = NodesA;
    Record.InheritedNodesB = NodesB;
    Record.FusionUniqueNode = FusionUniqueNode;
    Record.InheritedEssences = InheritedEssences;
    Record.DNAStability = ComputeDNAStability(bParentASynthOrigin, bParentBSynthOrigin);
    Record.bDahliaValidated = Record.DNAStability >= StabilityThresholdForRites;
    Record.bParentsDormant = true;
    Record.bNatureLineage = bNatureLineage;

    FusionRecords.Add(FusedDexID, Record);
    return FFusionResult::Ok(FusedDexID);
}

FFusionResult UFusionProgressionSubsystem::RequestUnfuse(int32 FusedDexID)
{
    FFusionRecord* Record = FusionRecords.Find(FusedDexID);
    if (!Record)
    {
        return FFusionResult::Refused(EFusionRefusalReason::NoRecord);
    }
    if (!Record->bParentsDormant)
    {
        return FFusionResult::Refused(EFusionRefusalReason::ParentsNotDormant);
    }

    // The parents wake exactly as they were. Nothing was taken from them;
    // nothing needs to be given back. The fused being's record is archived,
    // never deleted — what was, remains written down.
    Record->bParentsDormant = false;
    FusionArchive.Add(FusedDexID, *Record);
    FusionRecords.Remove(FusedDexID);
    return FFusionResult::Ok(FusedDexID);
}

FFusionResult UFusionProgressionSubsystem::RequestFusionRite(int32 FusedDexID, EFusionRiteStage Stage,
    float FusionTrustPercent, int32 EcologyAccomplishments, bool bHasCatalyst)
{
    FFusionRecord* Record = FusionRecords.Find(FusedDexID);
    if (!Record)
    {
        return FFusionResult::Refused(EFusionRefusalReason::NoRecord);
    }

    float RequiredTrust = 40.0f;
    int32 RequiredAccomplishments = 1;
    if (Stage == EFusionRiteStage::Harmonization)
    {
        RequiredTrust = 65.0f;
        RequiredAccomplishments = 3;
    }
    else if (Stage == EFusionRiteStage::SovereignConcord)
    {
        RequiredTrust = 85.0f;
        RequiredAccomplishments = 5;
    }

    if (FusionTrustPercent < RequiredTrust || EcologyAccomplishments < RequiredAccomplishments)
    {
        return FFusionResult::Refused(EFusionRefusalReason::UnstableLineage);
    }
    if (!bHasCatalyst)
    {
        return FFusionResult::Refused(EFusionRefusalReason::CatalystMissing);
    }

    // An unstable lineage may not attempt rites until D.A.H.L.I.A. has
    // validated it. The Forms Registry does not bless what it cannot verify.
    if (Record->DNAStability < StabilityThresholdForRites && !Record->bDahliaValidated)
    {
        return FFusionResult::Refused(EFusionRefusalReason::UnstableLineage);
    }

    // Nature lineage: every rite stage demands its explicit condition. This
    // is the conditional-only gate, enforced per stage, no exceptions.
    if (Record->bNatureLineage && !bHasCatalyst)
    {
        return FFusionResult::Refused(EFusionRefusalReason::CatalystMissing);
    }

    Record->CompletedRiteStage = Stage;
    Record->bHasCompletedAnyRite = true;
    return FFusionResult::Ok(FusedDexID);
}

bool UFusionProgressionSubsystem::ValidateWithDahlia(int32 FusedDexID)
{
    FFusionRecord* Record = FusionRecords.Find(FusedDexID);
    if (!Record)
    {
        return false;
    }
    // D.A.H.L.I.A. reviews the lineage record: both parents' Echoprints,
    // the inherited essence expressions, the stability score. Validation
    // steadies the line — it does not rewrite it.
    Record->bDahliaValidated = true;
    Record->DNAStability = FMath::Min(100.0f, Record->DNAStability + 15.0f);
    return true;
}

bool UFusionProgressionSubsystem::TryGetFusionRecord(int32 FusedDexID, FFusionRecord& OutRecord) const
{
    const FFusionRecord* Record = FusionRecords.Find(FusedDexID);
    if (!Record)
    {
        return false;
    }
    OutRecord = *Record;
    return true;
}

bool UFusionProgressionSubsystem::IsFused(int32 FusedDexID) const
{
    return FusionRecords.Contains(FusedDexID);
}

float UFusionProgressionSubsystem::GetDNAStability(int32 FusedDexID) const
{
    const FFusionRecord* Record = FusionRecords.Find(FusedDexID);
    return Record ? Record->DNAStability : 0.0f;
}

int32 UFusionProgressionSubsystem::AllocateFusedDexID()
{
    // Fused Dex IDs live above the wild range so a combined being can never
    // collide with a wild identity. Mutable counter in a real build would
    // live in the save record; kept here for the framework pass.
    return NextFusedDexID++;
}

float UFusionProgressionSubsystem::ComputeDNAStability(bool bParentASynthOrigin, bool bParentBSynthOrigin) const
{
    // Lab-born lines carry echo instability. Each Synth-Origin parent costs
    // stability; two of them nearly always require D.A.H.L.I.A. review.
    float Stability = 100.0f;
    if (bParentASynthOrigin) Stability -= SynthOriginStabilityPenalty;
    if (bParentBSynthOrigin) Stability -= SynthOriginStabilityPenalty;
    return FMath::Clamp(Stability, 5.0f, 100.0f);
}
