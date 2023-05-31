// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProject3.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h" // include this!


void IncludeShaderModule::StartupModule()
{
	//#if (ENGINE_MINOR_VERSION >= 21)
	FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders")); // add this!
	AddShaderSourceDirectoryMapping("/Project", ShaderDirectory);
	//#endif

}

void IncludeShaderModule::ShutdownModule() {}

//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, MyProject3, "MyProject3" );
IMPLEMENT_PRIMARY_GAME_MODULE(IncludeShaderModule, Project, "Project"); // edit this!
