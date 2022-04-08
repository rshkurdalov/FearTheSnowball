// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class FearTheSnowball : ModuleRules
{
	public FearTheSnowball(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks", "Core", "CoreUObject", "Engine", "InputCore", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "OnlineSubsystem" });
	}
}
