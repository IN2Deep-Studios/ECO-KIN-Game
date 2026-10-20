// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Progression/FinaleSubsystem.h"

void UFinaleSubsystem::SetGateOpen(bool bOpen)
{
    bGateOpen = bOpen;
}

bool UFinaleSubsystem::BeginFinale()
{
    if (!bGateOpen || CurrentStage != EFinaleStage::Dormant)
    {
        return false;
    }
    CurrentStage = EFinaleStage::Assembling;
    OnFinaleStage.Broadcast(CurrentStage);
    return true;
}

void UFinaleSubsystem::ReportStageComplete(EFinaleStage Stage)
{
    if (Stage != CurrentStage || CurrentStage == EFinaleStage::RestorationEra)
    {
        return;
    }
    // Advance one stage: Assembling -> Voyage -> EyeOfStorm ->
    // Purification -> RestorationEra.
    CurrentStage = static_cast<EFinaleStage>(static_cast<uint8>(CurrentStage) + 1);
    OnFinaleStage.Broadcast(CurrentStage);
}
