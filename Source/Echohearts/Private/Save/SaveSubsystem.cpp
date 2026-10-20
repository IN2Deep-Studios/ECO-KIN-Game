// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Save/SaveSubsystem.h"
#include "Save/EchoheartsSaveGame.h"
#include "Kismet/GameplayStatics.h"

FString USaveSubsystem::SlotNameFor(int32 SlotIndex) const
{
    return FString::Printf(TEXT("EchoheartsSlot%d"), SlotIndex);
}

void USaveSubsystem::NewGame()
{
    ActiveSave = Cast<UEchoheartsSaveGame>(UGameplayStatics::CreateSaveGameObject(UEchoheartsSaveGame::StaticClass()));
}

bool USaveSubsystem::SaveToSlot(int32 SlotIndex)
{
    if (!ActiveSave)
    {
        return false;
    }
    ActiveSave->SavedAt = FDateTime::Now();
    return UGameplayStatics::SaveGameToSlot(ActiveSave, SlotNameFor(SlotIndex), 0);
}

bool USaveSubsystem::LoadFromSlot(int32 SlotIndex)
{
    if (!DoesSlotExist(SlotIndex))
    {
        return false;
    }
    USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotNameFor(SlotIndex), 0);
    UEchoheartsSaveGame* Casted = Cast<UEchoheartsSaveGame>(Loaded);
    if (!Casted)
    {
        return false;
    }
    // Schema migration: older saves migrate forward here. Version 1 is current;
    // future versions add their upgrade path in this branch.
    if (Casted->SaveVersion > UEchoheartsSaveGame::CurrentSaveVersion)
    {
        return false; // save from a newer build — refuse rather than corrupt
    }
    ActiveSave = Casted;
    return true;
}

bool USaveSubsystem::DoesSlotExist(int32 SlotIndex) const
{
    return UGameplayStatics::DoesSaveGameExist(SlotNameFor(SlotIndex), 0);
}
