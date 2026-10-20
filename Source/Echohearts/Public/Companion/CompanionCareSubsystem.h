// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Companion/CompanionConditionTypes.h"
#include "CompanionCareSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnergyChanged, int32, EcoKinID, float, NewEnergy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAilmentGained, int32, EcoKinID, ECompanionAilment, Ailment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAilmentCleared, int32, EcoKinID, ECompanionAilment, Ailment);

// Companion Care: working companions get tired and sick; the tamer helps
// them rest, play, heal, and calm. Work drains Energy; at 0 the companion
// is Weary and can't work until rested. Working while Weary or in blight
// risks Ailing (needs treatment). Raids and heavy strain cause Shaken
// (needs calming). Care is gameplay: rest, play, mend, remedy, soothe.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UCompanionCareSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Registers a companion as working (harvesting, crafting, defending).
    // Work drains Energy on a tick while registered.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Care")
    void RegisterWorker(int32 EcoKinID);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Care")
    void UnregisterWorker(int32 EcoKinID);

    // Rest at a rest structure: restores Energy over the rest duration.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Care")
    bool RestCompanion(int32 EcoKinID, float HoursRested, float RecoveryPerHour);

    // Play at a fun station: restores Energy and a little joy (the game
    // can grant a small Kindling bump on OnPlayCompleted).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Care")
    bool PlayCompanion(int32 EcoKinID, float MinutesPlayed);

    // Treats an ailment with a care source. Returns false if the source
    // doesn't suit the ailment (e.g. play doesn't cure Ailing).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Care")
    bool TreatAilment(int32 EcoKinID, ECompanionAilment Ailment, ECareSource Source);

    // Calms a Shaken companion (Chime Grove, Gentle Melody, campfire...).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Care")
    bool SootheCompanion(int32 EcoKinID, ECareSource Source);

    // Blight exposure while working: risks Ailing, worse if already Weary.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Care")
    void ExposeToBlight(int32 EcoKinID);

    // Heavy strain event (raid, brutal duel): risks Shaken.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Care")
    void ApplyHeavyStrain(int32 EcoKinID);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Care")
    FCompanionCondition GetCondition(int32 EcoKinID) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Care")
    FOnEnergyChanged OnEnergyChanged;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Care")
    FOnAilmentGained OnAilmentGained;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Care")
    FOnAilmentCleared OnAilmentCleared;

    // Energy drained per second of registered work.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Care")
    float WorkDrainPerSecond = 0.35f;

private:
    void WorkTick();
    void GainAilment(int32 EcoKinID, ECompanionAilment Ailment);
    void ClearAilment(int32 EcoKinID, ECompanionAilment Ailment);

    UPROPERTY()
    TMap<int32, FCompanionCondition> Conditions;

    UPROPERTY()
    TSet<int32> Workers;

    FTimerHandle WorkTimerHandle;
    bool bTimerRunning = false;
};
