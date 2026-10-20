// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSubsystem.generated.h"

// Save slots 0-2 plus autosave. Writes are atomic: serialize to a temp object,
// then commit, so a crash mid-save never corrupts the slot.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API USaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Save")
    bool SaveToSlot(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Save")
    bool LoadFromSlot(int32 SlotIndex);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Save")
    bool DoesSlotExist(int32 SlotIndex) const;

    // The live save object gameplay writes into.
    UPROPERTY(BlueprintReadOnly, Category = "Echohearts|Save")
    class UEchoheartsSaveGame* ActiveSave = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Save")
    void NewGame();

private:
    FString SlotNameFor(int32 SlotIndex) const;
};
