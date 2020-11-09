// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Spaceport : ModuleRules
{
	private string ModulePath
	{
		get { return ModuleDirectory; }
	}

	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
	}

	public Spaceport(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        PublicIncludePaths.Add(Path.Combine(ModulePath, "../../../../", "boost_1_60_0"));
        PublicIncludePaths.Add(Path.Combine(ModulePath, "../../../../unreal_project/Spaceport/Source/Spaceport"));

        LoadProtobuf(Target);
        LoadJansson(Target);
    }

    public bool LoadProtobuf(ReadOnlyTargetRules Target)
    {
        string libraries_path = Path.Combine(ThirdPartyPath, "Protobuf", "Libraries");

        string protobuf_lib;
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            protobuf_lib = "libprotobuf.lib";
        }
        else // TODO: Untested!
        {
            protobuf_lib = "libprotobuf.a";
        }

        PublicAdditionalLibraries.Add(Path.Combine(libraries_path, protobuf_lib));
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Protobuf", "Includes"));
        //PublicDefinitions.Add(string.Format("USE_PROTOBUF=1"));
        PublicDefinitions.Add("GOOGLE_PROTOBUF_NO_RTTI");

        return true;
    }

    public bool LoadJansson(ReadOnlyTargetRules Target)
    {
        string libraries_path = Path.Combine(ThirdPartyPath, "Jansson", "Libraries");

        string jansson_lib;
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            jansson_lib = "jansson.lib";
        }
        else // TODO: Untested!
        {
            jansson_lib = "libjansson.a";
        }

        PublicAdditionalLibraries.Add(Path.Combine(libraries_path, jansson_lib));
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Jansson", "Includes"));
        //PublicDefinitions.Add(string.Format("USE_JANSSON=1"));

        return true;
    }
}
