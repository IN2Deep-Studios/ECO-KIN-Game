// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "EchoheartsHUD.generated.h"

// UI layers. The main HUD is always present in gameplay; everything else is
// a layer above it. Ceremony UI takes full focus — combat input is locked
// while a rite, Binding, or Release is underway.
UENUM(BlueprintType)
enum class EHUDLayer : uint8
{
    MainHUD      UMETA(DisplayName = "Main HUD"),       // vitals, link strain, companion health, essences
    Dex          UMETA(DisplayName = "Dex"),            // 125-ID Permanent Dex browser
    MissionLog   UMETA(DisplayName = "Mission Log"),    // mission journal
    Ceremony     UMETA(DisplayName = "Ceremony"),       // Binding / Growth Rite / Release
    DialogueUI   UMETA(DisplayName = "Dialogue"),       // NPC conversation
    PauseMenu    UMETA(DisplayName = "Pause Menu"),
    Championship UMETA(DisplayName = "Championship")    // broadcast overlay
};

// Owns UI layer visibility. Widget CLASSES are assigned in Blueprint (data),
// this HUD only decides what shows, when, and in what order.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API AEchoheartsHUD : public AHUD
{
    GENERATED_BODY()

public:
    AEchoheartsHUD();

    UFUNCTION(BlueprintCallable, Category = "Echohearts|UI")
    void ShowLayer(EHUDLayer Layer);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|UI")
    void HideLayer(EHUDLayer Layer);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|UI")
    void HideAllLayers();

    UFUNCTION(BlueprintPure, Category = "Echohearts|UI")
    bool IsLayerVisible(EHUDLayer Layer) const;

    // Widget classes, assigned in Blueprint. Keyed by layer.
    UPROPERTY(EditDefaultsOnly, Category = "Echohearts|UI")
    TMap<EHUDLayer, TSubclassOf<class UUserWidget>> LayerWidgetClasses;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TMap<EHUDLayer, class UUserWidget*> ActiveWidgets;
};
