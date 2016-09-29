// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class Spaceport : ModuleRules
{
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
    }

    public Spaceport(TargetInfo Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        // if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        // {
        //		if (UEBuildConfiguration.bCompileSteamOSS == true)
        //		{
        //			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //		}
        // }

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../../../../", "boost_1_60_0"));

        LoadProtobuf(Target);
        LoadJansson(Target);
        LoadASL(Target);
    }

    public bool LoadProtobuf(TargetInfo Target)
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
        //Definitions.Add(string.Format("USE_PROTOBUF=1"));
        Definitions.Add("GOOGLE_PROTOBUF_NO_RTTI");

        return true;
    }

    public bool LoadJansson(TargetInfo Target)
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
        //Definitions.Add(string.Format("USE_JANSSON=1"));

        return true;
    }

    public bool LoadASL(TargetInfo Target)
    {
        string libraries_path = Path.Combine(ThirdPartyPath, "ASL", "Libraries");

        string asl_lib;
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            asl_lib = "asl.lib";
        }
        else // TODO: Untested!
        {
            asl_lib = "libasl.a";
        }

        PublicAdditionalLibraries.Add(Path.Combine(libraries_path, asl_lib));

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../../../../", "adobe_source_libraries"));
        Definitions.Add(string.Format("ADOBE_FNV_NO_BIGINTS"));
        Definitions.Add(string.Format("ADOBE_NO_MSVC64_PROMOTION_ERROR"));

        return true;
    }
}
