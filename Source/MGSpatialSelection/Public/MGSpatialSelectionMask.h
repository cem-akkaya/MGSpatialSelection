// MG Spatial Selection System by Cem Akkaya https://www.cemakkaya.com

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MaterialExpressionIO.h"
#include "Materials/MaterialExpression.h"
#include "MGSpatialSelectionMask.generated.h"

/**
 * Material expression for generating a 3D Box Mask, used for terrain-aware selection highlights.
 */
UCLASS(MinimalAPI)
class UMGSpatialSelectionMask : public UMaterialExpression
{
	GENERATED_BODY()

public:
	UMGSpatialSelectionMask();

	/** The world position of the pixel, usually AbsoluteWorldPosition. */
	UPROPERTY()
	FExpressionInput WorldPos;

	/** The center of the selection box in world space. */
	UPROPERTY()
	FExpressionInput Center;

	/** The extent (half-size) of the selection box in world space. */
	UPROPERTY()
	FExpressionInput Extent;

	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual FText GetKeywords() const override { return FText::FromString(TEXT("selection, mask, box, mg")); }
#endif
	//~ End UMaterialExpression Interface
};
