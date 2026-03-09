#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterMacros.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphResources.h"

// Shader Parameters Structure
BEGIN_SHADER_PARAMETER_STRUCT(FMGSpatialSelectionShaderParameters, )
	SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FVector3f>, SurfacePoints)
	SHADER_PARAMETER(int32, NumSurfacePoints)
	SHADER_PARAMETER(int32, SelectionState)
END_SHADER_PARAMETER_STRUCT()

// Base Shader Class
class FMGSpatialSelectionShader : public FGlobalShader
{
public:
	FMGSpatialSelectionShader() {}
	FMGSpatialSelectionShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

// Vertex Shader
class FMGSpatialSelectionVS : public FMGSpatialSelectionShader
{
public:
	DECLARE_GLOBAL_SHADER(FMGSpatialSelectionVS);
	using FParameters = FMGSpatialSelectionShaderParameters;
	SHADER_USE_PARAMETER_STRUCT(FMGSpatialSelectionVS, FMGSpatialSelectionShader);
};

// Pixel Shader
class FMGSpatialSelectionPS : public FMGSpatialSelectionShader
{
public:
	DECLARE_GLOBAL_SHADER(FMGSpatialSelectionPS);
	using FParameters = FMGSpatialSelectionShaderParameters;
	SHADER_USE_PARAMETER_STRUCT(FMGSpatialSelectionPS, FMGSpatialSelectionShader);
};
