// Copyright Epic Games, Inc. All Rights Reserved. 
#include "MGSpatialSelection.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"
#include "Misc/Paths.h"
#define LOCTEXT_NAMESPACE "FMGSpatialSelectionModule"
void FMGSpatialSelectionModule::StartupModule() { FString ShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("MGSpatialSelection"))->GetBaseDir(), TEXT("Shaders")); AddShaderSourceDirectoryMapping(TEXT("/Plugin/MGSpatialSelection"), ShaderDir); } 
void FMGSpatialSelectionModule::ShutdownModule() {} 
#undef LOCTEXT_NAMESPACE 
IMPLEMENT_MODULE(FMGSpatialSelectionModule, MGSpatialSelection);
