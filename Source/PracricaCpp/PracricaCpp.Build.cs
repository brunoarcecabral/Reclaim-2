// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PracricaCpp : ModuleRules
{
	public PracricaCpp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"PracricaCpp",
			"PracricaCpp/Variant_Platforming",
			"PracricaCpp/Variant_Platforming/Animation",
			"PracricaCpp/Variant_Combat",
			"PracricaCpp/Variant_Combat/AI",
			"PracricaCpp/Variant_Combat/Animation",
			"PracricaCpp/Variant_Combat/Gameplay",
			"PracricaCpp/Variant_Combat/Interfaces",
			"PracricaCpp/Variant_Combat/UI",
			"PracricaCpp/Variant_SideScrolling",
			"PracricaCpp/Variant_SideScrolling/AI",
			"PracricaCpp/Variant_SideScrolling/Gameplay",
			"PracricaCpp/Variant_SideScrolling/Interfaces",
			"PracricaCpp/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
