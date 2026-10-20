// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "UI/EchoheartsHUD.h"
#include "Blueprint/UserWidget.h"

AEchoheartsHUD::AEchoheartsHUD()
{
}

void AEchoheartsHUD::BeginPlay()
{
    Super::BeginPlay();
    // Main HUD comes up with gameplay; everything else is summoned.
    ShowLayer(EHUDLayer::MainHUD);
}

void AEchoheartsHUD::ShowLayer(EHUDLayer Layer)
{
    if (ActiveWidgets.Contains(Layer))
    {
        if (UUserWidget* Existing = ActiveWidgets[Layer])
        {
            Existing->SetVisibility(ESlateVisibility::Visible);
        }
        return;
    }

    const TSubclassOf<UUserWidget>* ClassPtr = LayerWidgetClasses.Find(Layer);
    if (!ClassPtr || !*ClassPtr)
    {
        return; // unmapped layer: assigned in Blueprint, not a code error
    }

    if (UUserWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayerController(), *ClassPtr))
    {
        Widget->AddToViewport();
        ActiveWidgets.Add(Layer, Widget);
    }
}

void AEchoheartsHUD::HideLayer(EHUDLayer Layer)
{
    if (UUserWidget** Found = ActiveWidgets.Find(Layer))
    {
        if (*Found)
        {
            (*Found)->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void AEchoheartsHUD::HideAllLayers()
{
    for (auto& Pair : ActiveWidgets)
    {
        if (Pair.Value)
        {
            Pair.Value->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

bool AEchoheartsHUD::IsLayerVisible(EHUDLayer Layer) const
{
    const UUserWidget* const* Found = ActiveWidgets.Find(Layer);
    return Found && *Found && (*Found)->GetVisibility() == ESlateVisibility::Visible;
}
