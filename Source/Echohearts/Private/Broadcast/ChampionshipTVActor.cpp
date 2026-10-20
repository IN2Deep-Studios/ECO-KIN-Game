// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Broadcast/ChampionshipTVActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "FileMediaSource.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/Paths.h"

AChampionshipTV::AChampionshipTV()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    // Artist note: assign a real TV mesh (or a plane for the screen) in the
    // derived Blueprint or level instance; this code ships no mesh asset.
    Screen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
    Screen->SetupAttachment(Root);
}

void AChampionshipTV::BeginPlay()
{
    Super::BeginPlay();
    SetupMediaPipeline();
    // Auto-air the championship final on BeginPlay; loops until told otherwise.
    PlayReel(0);
}

void AChampionshipTV::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopBroadcast();
    Super::EndPlay(EndPlayReason);
}

void AChampionshipTV::SetupMediaPipeline()
{
    if (MediaPlayer == nullptr)
    {
        MediaPlayer = NewObject<UMediaPlayer>(this, TEXT("BroadcastPlayer"));
    }
    if (MediaTexture == nullptr)
    {
        MediaTexture = NewObject<UMediaTexture>(this, TEXT("BroadcastTexture"));
        if (MediaTexture != nullptr)
        {
            MediaTexture->SetMediaPlayer(MediaPlayer);
            MediaTexture->UpdateResource();
        }
    }
    if (MediaSource == nullptr)
    {
        MediaSource = NewObject<UFileMediaSource>(this, TEXT("BroadcastSource"));
    }
    BindMediaTextureToScreen();
}

void AChampionshipTV::BindMediaTextureToScreen()
{
    if (BaseBroadcastMaterial == nullptr)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("AChampionshipTV [%s]: no BaseBroadcastMaterial assigned. "
                 "Assign an unlit/emissive material with a 'VideoTexture' "
                 "parameter in the editor to show the broadcast picture."),
            *GetName());
        return;
    }
    if (Screen == nullptr || MediaTexture == nullptr)
    {
        return;
    }
    UMaterialInstanceDynamic* DynMat =
        UMaterialInstanceDynamic::Create(BaseBroadcastMaterial, this);
    if (DynMat != nullptr)
    {
        DynMat->SetTextureParameterValue(TEXT("VideoTexture"), MediaTexture);
        Screen->SetMaterial(0, DynMat);
    }
}

void AChampionshipTV::PlayReel(int32 ReelIndex)
{
    const TArray<FChampionshipReel> Reels = FChampionshipReels::GetDefaultReels();
    if (!Reels.IsValidIndex(ReelIndex))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("AChampionshipTV [%s]: PlayReel(%d) out of range (0..%d). Ignored."),
            *GetName(), ReelIndex, Reels.Num() - 1);
        return;
    }
    if (MediaPlayer == nullptr || MediaSource == nullptr)
    {
        SetupMediaPipeline();
    }

    const FChampionshipReel& Reel = Reels[ReelIndex];
    const FString FullPath =
        FPaths::ProjectContentDir() / TEXT("Movies") / Reel.MediaFileName;
    MediaSource->SetFilePath(FullPath);

    if (MediaPlayer->OpenSource(MediaSource))
    {
        CurrentReelIndex = ReelIndex;
        CurrentState = EBroadcastState::Airing;
        MediaPlayer->Play();
        MediaPlayer->OnEndReached.Clear();
        MediaPlayer->OnEndReached.AddUObject(this, &AChampionshipTV::HandleEndReached);
        UE_LOG(LogTemp, Log, TEXT("AChampionshipTV [%s]: airing reel %d '%s'."),
            *GetName(), ReelIndex, *Reel.Title);
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("AChampionshipTV [%s]: failed to open '%s'. Check Content/Movies/."),
            *GetName(), *FullPath);
        CurrentState = EBroadcastState::Off;
        CurrentReelIndex = INDEX_NONE;
    }
}

void AChampionshipTV::PlayNextReel()
{
    const int32 Count = FChampionshipReels::GetDefaultReels().Num();
    if (Count <= 0)
    {
        return;
    }
    const int32 Next = (CurrentReelIndex + 1) % Count;
    PlayReel(Next);
}

void AChampionshipTV::StopBroadcast()
{
    if (MediaPlayer != nullptr && MediaPlayer->IsPlaying())
    {
        MediaPlayer->Close();
    }
    CurrentState = EBroadcastState::Off;
    CurrentReelIndex = INDEX_NONE;
}

void AChampionshipTV::HandleEndReached()
{
    // Broadcast loop: replay the current reel when it ends.
    if (CurrentState == EBroadcastState::Airing && MediaPlayer != nullptr)
    {
        MediaPlayer->Play();
    }
}
