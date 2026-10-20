// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "EcoKin/EcoKinDexSubsystem.h"
#include "Echohearts.h"

FEcoResult FEcoResult::Ok()
{
    FEcoResult Result;
    Result.bSucceeded = true;
    Result.ErrorCode = NAME_None;
    return Result;
}

FEcoResult FEcoResult::Fail(FName InErrorCode, const FText& InMessage)
{
    FEcoResult Result;
    Result.bSucceeded = false;
    Result.ErrorCode = InErrorCode;
    Result.Message = InMessage;
    return Result;
}

bool UEcoKinDexSubsystem::IsProtectedCoreID(int32 EcoKinID)
{
    return EcoKinID >= 1 && EcoKinID <= ProtectedCoreDexMax;
}

FEcoResult UEcoKinDexSubsystem::RegisterIdentity(const FEcoKinIdentity& Identity)
{
    if (Identity.EcoKinID <= 0)
    {
        return FEcoResult::Fail(
            FName(TEXT("EcoKin.Dex.InvalidID")),
            FText::FromString(TEXT("EcoKin ID must be a positive Dex number.")));
    }

    if (Identity.Name.IsNone())
    {
        return FEcoResult::Fail(
            FName(TEXT("EcoKin.Dex.MissingName")),
            FText::FromString(TEXT("EcoKin identity requires a name.")));
    }

    if (Identity.EchoprintHash.IsEmpty())
    {
        // Locked canon: every Eco-Kin has an immutable Echoprint.
        return FEcoResult::Fail(
            FName(TEXT("EcoKin.Dex.MissingEchoprint")),
            FText::FromString(TEXT("EcoKin identity requires an Echoprint hash; approved appearance is never silent.")));
    }

    if (ActiveDex.Contains(Identity.EcoKinID))
    {
        const FEcoKinIdentity& Existing = ActiveDex[Identity.EcoKinID];
        if (Existing.EchoprintHash != Identity.EchoprintHash)
        {
            // Protected: an approved Echoprint is never silently replaced.
            UE_LOG(LogEchohearts, Warning, TEXT("RegisterIdentity: rejected Echoprint change for Dex %d."), Identity.EcoKinID);
            return FEcoResult::Fail(
                FName(TEXT("EcoKin.Dex.EchoprintLocked")),
                FText::FromString(TEXT("Echoprint is permanent; this Dex ID already has a locked appearance.")));
        }
        // Re-registering the identical record is idempotent.
        return FEcoResult::Ok();
    }

    ActiveDex.Add(Identity.EcoKinID, Identity);
    return FEcoResult::Ok();
}

FEcoResult UEcoKinDexSubsystem::AttachForm(const FEcoformRecord& FormRecord)
{
    if (!ActiveDex.Contains(FormRecord.BaseEcoKinID))
    {
        return FEcoResult::Fail(
            FName(TEXT("EcoKin.Forms.UnknownBase")),
            FText::FromString(TEXT("Forms attach to a registered base EcoKin identity; the base is not registered.")));
    }

    if (FormRecord.Kind == EEcoformKind::ShimmerForm && !FormRecord.bPreservesIdentity)
    {
        // Locked canon: Shimmer Form is the universal ascension and preserves identity.
        return FEcoResult::Fail(
            FName(TEXT("EcoKin.Forms.ShimmerMustPreserveIdentity")),
            FText::FromString(TEXT("Shimmer Form must preserve the base EcoKin identity.")));
    }

    TArray<FEcoformRecord>& Forms = FormsRegistry.FindOrAdd(FormRecord.BaseEcoKinID);
    for (const FEcoformRecord& Existing : Forms)
    {
        if (Existing.FormID == FormRecord.FormID)
        {
            return FEcoResult::Ok(); // Idempotent re-attach.
        }
    }
    Forms.Add(FormRecord);
    return FEcoResult::Ok();
}

FEcoResult UEcoKinDexSubsystem::SetCarriedSquad(const TArray<int32>& EcoKinIDs)
{
    if (EcoKinIDs.Num() > MaxCarriedSquadSize)
    {
        return FEcoResult::Fail(
            FName(TEXT("EcoKin.Squad.OverCapacity")),
            FText::FromString(TEXT("Carried squad contract: at most 8 Eco-Kin.")));
    }

    for (int32 EcoKinID : EcoKinIDs)
    {
        if (!ActiveDex.Contains(EcoKinID))
        {
            return FEcoResult::Fail(
                FName(TEXT("EcoKin.Squad.UnknownIdentity")),
                FText::FromString(TEXT("Carried squad may only contain registered Dex identities.")));
        }
    }

    CarriedSquad = EcoKinIDs;
    ActiveSquad.RemoveAll([this](int32 EcoKinID) { return !CarriedSquad.Contains(EcoKinID); });
    return FEcoResult::Ok();
}

FEcoResult UEcoKinDexSubsystem::SetActiveSquad(const TArray<int32>& EcoKinIDs)
{
    if (EcoKinIDs.Num() > MaxActiveSquadSize)
    {
        return FEcoResult::Fail(
            FName(TEXT("EcoKin.Squad.OverCapacity")),
            FText::FromString(TEXT("Active squad contract: at most 3 Eco-Kin.")));
    }

    for (int32 EcoKinID : EcoKinIDs)
    {
        if (!CarriedSquad.Contains(EcoKinID))
        {
            return FEcoResult::Fail(
                FName(TEXT("EcoKin.Squad.NotCarried")),
                FText::FromString(TEXT("Active squad members must be in the carried squad.")));
        }
    }

    ActiveSquad = EcoKinIDs;
    return FEcoResult::Ok();
}

bool UEcoKinDexSubsystem::TryGetIdentity(int32 EcoKinID, FEcoKinIdentity& OutIdentity) const
{
    if (const FEcoKinIdentity* Found = ActiveDex.Find(EcoKinID))
    {
        OutIdentity = *Found;
        return true;
    }
    return false;
}

TArray<FEcoformRecord> UEcoKinDexSubsystem::GetFormsFor(int32 EcoKinID) const
{
    if (const TArray<FEcoformRecord>* Found = FormsRegistry.Find(EcoKinID))
    {
        return *Found;
    }
    return TArray<FEcoformRecord>();
}
