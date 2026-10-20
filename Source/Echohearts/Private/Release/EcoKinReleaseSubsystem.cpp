// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Release/EcoKinReleaseSubsystem.h"
#include "Echohearts.h"

// RELEASE IS NOT SACRIFICE. This subsystem only ever handles resources an
// EcoKin willingly shed or gifted, or plants the living world offers. There is
// no code path here that kills, harms, or consumes a sentient partner.

FEcoResult UEcoKinReleaseSubsystem::ReleaseEcoKin(
    const FEcoKinIdentity& Identity, bool bWilling, FReleaseYield& OutYield)
{
    OutYield = FReleaseYield();

    if (!bWilling)
    {
        return FEcoResult::Fail(
            FName(TEXT("Release.NotWilling")),
            FText::FromString(TEXT("Release refused: the EcoKin did not willingly depart. No resources granted.")));
    }

    if (Identity.EcoKinID <= 0 || Identity.Name.IsNone())
    {
        return FEcoResult::Fail(
            FName(TEXT("Release.InvalidIdentity")),
            FText::FromString(TEXT("Release requires a registered EcoKin identity.")));
    }

    // The EcoKin returns to the wild. Only what it chose to leave behind is kept.
    BuildReleaseTable(Identity, OutYield);
    return FEcoResult::Ok();
}

FEcoResult UEcoKinReleaseSubsystem::RegisterFloraNode(const FHarvestNode& Node)
{
    if (Node.NodeId.IsNone() || Node.ResourceId.IsNone())
    {
        return FEcoResult::Fail(
            FName(TEXT("Release.Node.Invalid")),
            FText::FromString(TEXT("Flora node requires a node ID and a resource ID.")));
    }
    if (Node.YieldPerHarvest <= 0)
    {
        return FEcoResult::Fail(
            FName(TEXT("Release.Node.EmptyYield")),
            FText::FromString(TEXT("Flora node yield must be positive.")));
    }

    FloraNodes.Add(Node.NodeId, Node);
    return FEcoResult::Ok();
}

FEcoResult UEcoKinReleaseSubsystem::HarvestNode(FName NodeId, FReleaseYield& OutYield)
{
    OutYield = FReleaseYield();

    const FHarvestNode* Node = FloraNodes.Find(NodeId);
    if (!Node)
    {
        return FEcoResult::Fail(
            FName(TEXT("Release.Node.Unknown")),
            FText::FromString(TEXT("Unknown flora node.")));
    }

    const double NowSeconds = FPlatformTime::Seconds();
    if (const double* LastHarvest = LastHarvestSecondsByNode.Find(NodeId))
    {
        if (NowSeconds - *LastHarvest < static_cast<double>(Node->RespawnSeconds))
        {
            return FEcoResult::Fail(
                FName(TEXT("Release.Node.Regrowing")),
                FText::FromString(TEXT("This flora is still regrowing. The world replenishes; it is never stripped bare.")));
        }
    }

    FResourceStack Stack;
    Stack.ResourceId = Node->ResourceId;
    Stack.DisplayName = Node->DisplayName;
    Stack.SourceOrder = EEcoKinOrder::Flora;
    Stack.SourceEssence = EEcoKinEssence::Flora;
    Stack.Quantity = Node->YieldPerHarvest;
    StampStack(Stack, Node->PointsPerUnit);

    OutYield.AddStack(Stack);
    LastHarvestSecondsByNode.Add(NodeId, NowSeconds);
    return FEcoResult::Ok();
}

int32 UEcoKinReleaseSubsystem::GetLifetimePoints(FName ResourceId) const
{
    const int32* Points = LifetimePointsByResource.Find(ResourceId);
    return Points ? *Points : 0;
}

EResourceStarTier UEcoKinReleaseSubsystem::GetStarTier(FName ResourceId) const
{
    return GetStarTierForLifetimePoints(GetLifetimePoints(ResourceId));
}

FString UEcoKinReleaseSubsystem::EssenceAdjective(EEcoKinEssence Essence)
{
    switch (Essence)
    {
    case EEcoKinEssence::Flora:  return TEXT("Verdant");
    case EEcoKinEssence::Torrent: return TEXT("Tidewashed");
    case EEcoKinEssence::Pyre:   return TEXT("Emberkissed");
    case EEcoKinEssence::Terra:  return TEXT("Stoneworn");
    case EEcoKinEssence::Aero:   return TEXT("Windborne");
    case EEcoKinEssence::Glaze:  return TEXT("Frosttouched");
    case EEcoKinEssence::Voltic: return TEXT("Stormcharged");
    case EEcoKinEssence::Aura:    return TEXT("Luminous");
    case EEcoKinEssence::Shade:  return TEXT("Duskveiled");
    default:                      return TEXT("Wild");
    }
}

void UEcoKinReleaseSubsystem::StampStack(FResourceStack& Stack, float PointsPerUnit)
{
    const int32 Gained = FMath::Max(1, FMath::RoundToInt(static_cast<float>(Stack.Quantity) * PointsPerUnit));
    int32& Lifetime = LifetimePointsByResource.FindOrAdd(Stack.ResourceId);
    Lifetime += Gained;
    Stack.LifetimePoints = Lifetime;
    Stack.StarTier = GetStarTierForLifetimePoints(Lifetime);
}

void UEcoKinReleaseSubsystem::BuildReleaseTable(
    const FEcoKinIdentity& Identity, FReleaseYield& OutYield) const
{
    const FString Flavor = EssenceAdjective(Identity.PrimaryEssence);

    // NOTE: const_cast is used only to stamp lifetime progress; the table itself
    // is deterministic per identity. Nothing here touches the EcoKin.
    UEcoKinReleaseSubsystem* MutableThis = const_cast<UEcoKinReleaseSubsystem*>(this);

    auto Emit = [&](const FString& ResourceIdStr, const FString& BaseName,
        int32 Quantity, float PointsPerUnit)
    {
        FResourceStack Stack;
        Stack.ResourceId = FName(*ResourceIdStr);
        Stack.DisplayName = FText::FromString(Flavor + TEXT(" ") + BaseName);
        Stack.SourceOrder = Identity.Order;
        Stack.SourceEssence = Identity.PrimaryEssence;
        Stack.Quantity = Quantity;
        MutableThis->StampStack(Stack, PointsPerUnit);
        OutYield.AddStack(Stack);
    };

    switch (Identity.Order)
    {
    case EEcoKinOrder::Zoa:
        Emit(TEXT("Zoa.ShedHide"), TEXT("Shed Hide"), 4, 1.0f);
        Emit(TEXT("Zoa.MoltedScale"), TEXT("Molted Scale"), 3, 2.0f);
        Emit(TEXT("Zoa.FallenFeather"), TEXT("Fallen Feather"), 2, 2.0f);
        break;
    case EEcoKinOrder::Flora:
        Emit(TEXT("Flora.FallenPetal"), TEXT("Fallen Petal"), 6, 1.0f);
        Emit(TEXT("Flora.OfferedSap"), TEXT("Offered Sap"), 3, 2.0f);
        Emit(TEXT("Flora.DriftedSpore"), TEXT("Drifted Spore"), 2, 2.0f);
        break;
    case EEcoKinOrder::Myko:
        Emit(TEXT("Myko.DriftedSpore"), TEXT("Drifted Spore"), 4, 1.5f);
        Emit(TEXT("Myko.EssenceTrace"), TEXT("Essence Trace"), 2, 3.0f);
        break;
    case EEcoKinOrder::Proto:
        Emit(TEXT("Proto.LoosedCrystalShard"), TEXT("Loosed Crystal Shard"), 3, 3.0f);
        break;
    case EEcoKinOrder::Monera:
        Emit(TEXT("Monera.ShedBioEssence"), TEXT("Shed Bio-Essence"), 2, 4.0f);
        break;
    default:
        break;
    }
}
