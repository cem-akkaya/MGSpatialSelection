// Copyright Epic Games, Inc. All Rights Reserved.

#include "MGSpatialSelection.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FMGSpatialSelectionModule"

void FMGSpatialSelectionModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FString ShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("MGSpatialSelection"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/MGSpatialSelection"), ShaderDir);
}

void FMGSpatialSelectionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMGSpatialSelectionModule, MGSpatialSelection)