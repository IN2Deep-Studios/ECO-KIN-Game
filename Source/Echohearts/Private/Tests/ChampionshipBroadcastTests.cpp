// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#if WITH_DEV_AUTOMATION_TESTS

#include "Broadcast/ChampionshipTypes.h"
#include "Misc/AutomationTest.h"

// Championship broadcast reels are recordings of willing EcoKin exhibition
// matches: no capture, coercion, or sacrifice concepts exist in this system.
// EVIDENCE TIER: IMPLEMENTED only. NOT COMPILED, NOT TESTED, NOT RUNTIME VERIFIED.

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FChampionshipReelsCatalogTest,
    "Echohearts.Contracts.Broadcast.ReelsCatalog",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FChampionshipReelsCatalogTest::RunTest(const FString& Parameters)
{
    const TArray<FChampionshipReel> Reels = FChampionshipReels::GetDefaultReels();

    TestEqual(TEXT("Default catalog has 4 reels"), Reels.Num(), 4);

    TSet<FString> SeenTitles;
    for (const FChampionshipReel& Reel : Reels)
    {
        TestFalse(TEXT("Reel title is non-empty"), Reel.Title.IsEmpty());
        TestFalse(TEXT("Media filename is non-empty"), Reel.MediaFileName.IsEmpty());
        TestTrue(TEXT("Media filename ends in .mp4"),
            Reel.MediaFileName.EndsWith(TEXT(".mp4"), ESearchCase::IgnoreCase));
        TestFalse(TEXT("Featured EcoKin A is non-empty"), Reel.FeaturedEcoKinA.IsEmpty());
        TestFalse(TEXT("Featured EcoKin B is non-empty"), Reel.FeaturedEcoKinB.IsEmpty());

        bool bAlreadySeen = false;
        SeenTitles.Add(Reel.Title, &bAlreadySeen);
        TestFalse(TEXT("Reel titles are unique"), bAlreadySeen);
    }

    TestTrue(TEXT("Reel 0 is the championship final"),
        Reels[0].Title.Contains(TEXT("Terra Tusk")) && Reels[0].Title.Contains(TEXT("Apollo")));
    TestTrue(TEXT("Reel 1 is the semi-final"),
        Reels[1].Title.Contains(TEXT("Zangoro Seedling")) && Reels[1].Title.Contains(TEXT("Stormcrown Drakyr")));
    TestTrue(TEXT("Reel 2 is the showcase"),
        Reels[2].Title.Contains(TEXT("Lyravine")) && Reels[2].Title.Contains(TEXT("Pyrolith")));
    TestTrue(TEXT("Reel 3 is the opening ceremony"),
        Reels[3].Title.Contains(TEXT("Emperor Moth")) && Reels[3].Title.Contains(TEXT("Prismavex")));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FBroadcastStateEnumTest,
    "Echohearts.Contracts.Broadcast.StateEnum",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FBroadcastStateEnumTest::RunTest(const FString& Parameters)
{
    // Enum sanity: Off, Airing, and Paused are three distinct states.
    TestTrue(TEXT("Off != Airing"), EBroadcastState::Off != EBroadcastState::Airing);
    TestTrue(TEXT("Airing != Paused"), EBroadcastState::Airing != EBroadcastState::Paused);
    TestTrue(TEXT("Off != Paused"), EBroadcastState::Off != EBroadcastState::Paused);
    TestEqual(TEXT("Off is the zero value"),
        static_cast<uint8>(EBroadcastState::Off), static_cast<uint8>(0));
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
