// MG Spatial Selection System by Cem Akkaya https://www.cemakkaya.com

#include "MGSpatialSelectionMask.h"

#if WITH_EDITOR
#include "MaterialCompiler.h"
#endif

UMGSpatialSelectionMask::UMGSpatialSelectionMask()
{
#if WITH_EDITORONLY_DATA
	MenuCategories.Add(FText::FromString(TEXT("MG Selection")));
#endif
}

#if WITH_EDITOR
int32 UMGSpatialSelectionMask::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	if (!WorldPos.GetTracedInput().Expression)
	{
		return Compiler->Errorf(TEXT("Missing input WorldPos"));
	}
	else if (!Center.GetTracedInput().Expression)
	{
		return Compiler->Errorf(TEXT("Missing input Center"));
	}
	else if (!Extent.GetTracedInput().Expression)
	{
		return Compiler->Errorf(TEXT("Missing input Extent"));
	}
	else
	{
		// 1. Get the inputs
		int32 PosInput = WorldPos.Compile(Compiler);
		int32 CenterInput = Center.Compile(Compiler);
		int32 ExtentInput = Extent.Compile(Compiler);

		// Box Mask Logic: d = abs(WorldPos - Center) - Extent
		int32 Delta = Compiler->Sub(PosInput, CenterInput);
		int32 AbsDelta = Compiler->Abs(Delta);
		int32 Dist = Compiler->Sub(AbsDelta, ExtentInput);

		// Result = max(d.x, max(d.y, d.z)) < 0.0
		// Extract individual components
		int32 DX = Compiler->ComponentMask(Dist, true, false, false, false);
		int32 DY = Compiler->ComponentMask(Dist, false, true, false, false);
		int32 DZ = Compiler->ComponentMask(Dist, false, false, true, false);

		// Find the max of all components
		int32 MaxD = Compiler->Max(DX, Compiler->Max(DY, DZ));

		// Step(0.0, -MaxD) will return 1.0 if -MaxD >= 0 (meaning MaxD <= 0)
		int32 NegativeMaxD = Compiler->Mul(MaxD, Compiler->Constant(-1.0f));
		return Compiler->Step(Compiler->Constant(0.0f), NegativeMaxD);
	}
}

void UMGSpatialSelectionMask::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(TEXT("MG Spatial Selection Mask"));
}
#endif
