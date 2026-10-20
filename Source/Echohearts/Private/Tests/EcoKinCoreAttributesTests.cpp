#if WITH_DEV_AUTOMATION_TESTS

#include "EcoKin/EcoKinCoreAttributes.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEcoKinCoreAttributesClampTest,
    "Echohearts.EcoKin.Attributes.ClampToCanonicalRange",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FEcoKinCoreAttributesClampTest::RunTest(const FString& Parameters)
{
    FEcoKinCoreAttributes Attributes;
    Attributes.Vibrance = -25.0f;
    Attributes.Density = 125.0f;
    Attributes.Harmony = 101.0f;
    Attributes.Purity = -1.0f;

    Attributes.Clamp();

    TestEqual(TEXT("Vibrance clamps to minimum"), Attributes.Vibrance, 0.0f);
    TestEqual(TEXT("Density clamps to maximum"), Attributes.Density, 100.0f);
    TestEqual(TEXT("Harmony clamps to maximum"), Attributes.Harmony, 100.0f);
    TestEqual(TEXT("Purity clamps to minimum"), Attributes.Purity, 0.0f);

    return true;
}

#endif
