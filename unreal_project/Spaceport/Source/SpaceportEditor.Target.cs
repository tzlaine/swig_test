// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SpaceportEditorTarget : TargetRules
{
    public SpaceportEditorTarget( TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange( new string[] { "Spaceport" } );
        CppStandard = CppStandardVersion.Cpp20;
        bLegacyParentIncludePaths = false;
        bValidateFormatStrings = true;
        if (Target.Platform == UnrealTargetPlatform.Win64) {
            WindowsPlatform.bStrictConformanceMode = true;
        }
    }
}
