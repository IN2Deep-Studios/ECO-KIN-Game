using UnrealBuildTool;

public class Echohearts : ModuleRules
{
    public Echohearts(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "EnhancedInput",
                "GameplayTags",
                "GameplayTasks",
                "MediaAssets",
                "AIModule"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "UMG",
                "Slate",
                "SlateCore",
                "AnimationCore",
                "AnimGraphRuntime"
            });
    }
}
