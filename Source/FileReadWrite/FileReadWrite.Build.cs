// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class FileReadWrite : ModuleRules
{
    public FileReadWrite(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] 
            {  
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] 
            {
                "Core",
				"CoreUObject", 
				"Engine", 
				"InputCore",
				"Json", 
				"JsonUtilities"
            }
        );
    }
}
