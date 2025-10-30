// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SpaceportEditorTarget : TargetRules
{
	public SpaceportEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
                bOverrideBuildEnvironment = true;
		ExtraModuleNames.AddRange( new string[] { "Spaceport" } );
		CppStandard = CppStandardVersion.Cpp20;
    }
}
