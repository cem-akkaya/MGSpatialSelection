// MG Spatial Selection System by Cem Akkaya https://www.cemakkaya.com

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MGSpatialSelectionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UMGSpatialSelectionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that can be selected by the MG Spatial Selection System.
 */
class MGSPATIALSELECTION_API IMGSpatialSelectionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** Called when the actor's selection status changes. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MG Spatial Selection")
	void OnSelectionStatus(bool bIsSelected);
};
