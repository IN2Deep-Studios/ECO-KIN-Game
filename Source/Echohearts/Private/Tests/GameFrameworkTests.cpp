#if WITH_DEV_AUTOMATION_TESTS

#include "Tamer/TamerTypes.h"
#include "Mission/MissionTypes.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTamerVitalsClampTest,
    "Echohearts.Tamer.Vitals.ClampAndExhaustion",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FTamerVitalsClampTest::RunTest(const FString& Parameters)
{
    FTamerVitals Vitals;
    Vitals.Stamina = -10.0f;
    Vitals.Health = 140.0f;
    Vitals.Clamp();

    TestEqual(TEXT("Stamina clamps to minimum"), Vitals.Stamina, 0.0f);
    TestEqual(TEXT("Health clamps to maximum"), Vitals.Health, 100.0f);
    TestTrue(TEXT("Zero stamina reads as exhausted"), Vitals.IsExhausted());
    TestFalse(TEXT("Full health is not down"), Vitals.IsDown());

    Vitals.Health = 0.0f;
    TestTrue(TEXT("Zero health reads as down"), Vitals.IsDown());

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FMissionObjectiveTest,
    "Echohearts.Mission.Objectives.CompleteWhenAllDone",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMissionObjectiveTest::RunTest(const FString& Parameters)
{
    FMission Mission;
    Mission.MissionId = FName(TEXT("RestoreTest"));

    FMissionObjective O1;
    O1.TargetCount = 2;
    O1.CurrentCount = 2;
    FMissionObjective O2;
    O2.TargetCount = 1;
    O2.CurrentCount = 0;
    Mission.Objectives.Add(O1);
    Mission.Objectives.Add(O2);

    TestFalse(TEXT("Mission incomplete while an objective is open"), Mission.AllObjectivesDone());

    Mission.Objectives[1].CurrentCount = 1;
    TestTrue(TEXT("Mission complete when all objectives done"), Mission.AllObjectivesDone());

    FMission Empty;
    TestFalse(TEXT("Mission with no objectives is not complete"), Empty.AllObjectivesDone());

    return true;
}

#endif
