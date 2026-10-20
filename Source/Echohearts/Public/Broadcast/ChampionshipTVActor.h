// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Broadcast/ChampionshipTypes.h"
#include "ChampionshipTVActor.generated.h"

class UMediaPlayer;
class UMediaTexture;
class UFileMediaSource;
class UStaticMeshComponent;
class USceneComponent;
class UMaterialInterface;

// AChampionshipTV: an in-world television that airs championship exhibition
// reels. No capture, coercion, or sacrifice anywhere in this system; the reels
// are recordings of willing EcoKin exhibition matches.
//
// ARTIST INTEGRATION POINTS (no .uasset shipped with this code):
// - Assign a real TV/static mesh to the Screen component (or a child frame
//   mesh) in the derived Blueprint or level instance.
// - Set BaseBroadcastMaterial to an UNLIT/EMISSIVE material with a Texture
//   parameter named "VideoTexture". The media texture is pushed into that
//   parameter at runtime via a dynamic material instance.
// - MP4s live at <Project>/Content/Movies/ and are opened via UFileMediaSource
//   (FileMediaSource-compatible game assets, NOT .uassets).
//
// EVIDENCE TIER: IMPLEMENTED only. NOT COMPILED, NOT TESTED, NOT RUNTIME VERIFIED.
UCLASS(Blueprintable)
class ECHOHEARTS_API AChampionshipTV : public AActor
{
    GENERATED_BODY()

public:
    AChampionshipTV();

    // Emissive base material with a "VideoTexture" texture parameter. If left
    // null, the TV still plays audio/video state logic but logs a warning and
    // shows no picture: assign one in the editor.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Broadcast")
    UMaterialInterface* BaseBroadcastMaterial = nullptr;

    // The visible screen. Assign a TV/plane static mesh in the editor.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echohearts|Broadcast")
    UStaticMeshComponent* Screen = nullptr;

    // Starts the given reel by index into FChampionshipReels::GetDefaultReels().
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Broadcast")
    void PlayReel(int32 ReelIndex);

    // Advances to the next reel, wrapping around the catalog.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Broadcast")
    void PlayNextReel();

    // Stops playback and returns the TV to the Off state.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Broadcast")
    void StopBroadcast();

    UFUNCTION(BlueprintPure, Category = "Echohearts|Broadcast")
    EBroadcastState GetCurrentState() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category = "Echohearts|Broadcast")
    int32 GetCurrentReelIndex() const { return CurrentReelIndex; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY(Transient)
    USceneComponent* Root = nullptr;

    // Created at runtime in BeginPlay (NewObject); no media .uassets required.
    UPROPERTY(Transient)
    UMediaPlayer* MediaPlayer = nullptr;

    UPROPERTY(Transient)
    UMediaTexture* MediaTexture = nullptr;

    UPROPERTY(Transient)
    UFileMediaSource* MediaSource = nullptr;

    EBroadcastState CurrentState = EBroadcastState::Off;
    int32 CurrentReelIndex = INDEX_NONE;

    void SetupMediaPipeline();
    void BindMediaTextureToScreen();
    void HandleEndReached();
};
