// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Broadcast/ChampionshipTypes.generated.h"

// CANON LOCK: Championship battles are EXHIBITION SPORT. Every competing EcoKin
// chooses to compete; there is no coercion, no capture-ball language, and no
// sacrifice/consumption of any EcoKin anywhere in this system.
// EVIDENCE TIER: IMPLEMENTED only. NOT COMPILED, NOT TESTED, NOT RUNTIME VERIFIED.

UENUM(BlueprintType)
enum class EBroadcastState : uint8
{
    // TV is idle: no media loaded or playback stopped by design.
    Off     UMETA(DisplayName = "Off"),
    // A championship reel is currently playing on this TV.
    Airing  UMETA(DisplayName = "Airing"),
    // Playback is paused; the reel can resume without reloading.
    Paused  UMETA(DisplayName = "Paused")
};

// One championship broadcast reel: a recorded exhibition match between two
// willing EcoKin competitors, backed by an MP4 in Content/Movies/ (game asset,
// FileMediaSource-compatible; NOT a .uasset).
USTRUCT(BlueprintType)
struct ECHOHEARTS_API FChampionshipReel
{
    GENERATED_BODY()

    // Display title shown on the broadcast (e.g. "Championship Final: ...").
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Broadcast")
    FString Title;

    // Exact MP4 filename under <Project>/Content/Movies/ (no path, no subfolder).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Broadcast")
    FString MediaFileName;

    // Names of the two competing EcoKin (exhibition, both willing).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Broadcast")
    FString FeaturedEcoKinA;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Broadcast")
    FString FeaturedEcoKinB;

    // One-line broadcast blurb.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echohearts|Broadcast")
    FString Blurb;
};

// Default championship reel catalog shipped with the game.
struct ECHOHEARTS_API FChampionshipReels
{
    static TArray<FChampionshipReel> GetDefaultReels()
    {
        TArray<FChampionshipReel> Reels;

        FChampionshipReel Final;
        Final.Title = TEXT("Championship Final: Terra Tusk vs Apollo");
        Final.MediaFileName = TEXT("media-generation-championship-tv-terra-tusk-vs-0-cf030b17-7151-4b26-8646-433621c98fd9.mp4");
        Final.FeaturedEcoKinA = TEXT("Terra Tusk");
        Final.FeaturedEcoKinB = TEXT("Apollo");
        Final.Blurb = TEXT("The championship final: a seismic colossus meets a storm-winged lion.");
        Reels.Add(MoveTemp(Final));

        FChampionshipReel SemiFinal;
        SemiFinal.Title = TEXT("Semi-Final: Zangoro Seedling vs Stormcrown Drakyr");
        SemiFinal.MediaFileName = TEXT("media-generation-championship-tv-zangoro-vs-sto-0-5b48c3c5-03e0-4ecd-bc70-68b609eeb8fb.mp4");
        SemiFinal.FeaturedEcoKinA = TEXT("Zangoro Seedling");
        SemiFinal.FeaturedEcoKinB = TEXT("Stormcrown Drakyr");
        SemiFinal.Blurb = TEXT("A lightning duel in a rain-swept arena: axe-wielder against storm-crowned dragon.");
        Reels.Add(MoveTemp(SemiFinal));

        FChampionshipReel Showcase;
        Showcase.Title = TEXT("Showcase: Lyravine vs Pyrolith");
        Showcase.MediaFileName = TEXT("media-generation-championship-tv-lyravine-vs-py-0-8d0c3be4-ba1b-4386-80eb-b0603a0d2f06.mp4");
        Showcase.FeaturedEcoKinA = TEXT("Lyravine");
        Showcase.FeaturedEcoKinB = TEXT("Pyrolith");
        Showcase.Blurb = TEXT("Water and fire in exhibition: the living current against the ashbloom kin.");
        Reels.Add(MoveTemp(Showcase));

        FChampionshipReel Ceremony;
        Ceremony.Title = TEXT("Opening Ceremony: Emperor Moth vs Prismavex");
        Ceremony.MediaFileName = TEXT("media-generation-championship-tv-moth-duel-0-5a575893-c6eb-4a82-9769-f57c916d68ff.mp4");
        Ceremony.FeaturedEcoKinA = TEXT("Emperor Moth");
        Ceremony.FeaturedEcoKinB = TEXT("Prismavex");
        Ceremony.Blurb = TEXT("The opening ceremony: an aerial duel of moonlight and rainbow wings.");
        Reels.Add(MoveTemp(Ceremony));

        return Reels;
    }
};
