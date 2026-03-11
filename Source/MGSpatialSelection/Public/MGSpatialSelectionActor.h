// MG Spatial Selection System by Cem Akkaya https://www.cemakkaya.com

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGSpatialSelectionActor.generated.h"

class UMGSpatialSelectionComponent;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectionActorFinished);

UCLASS()
class MGSPATIALSELECTION_API AMGSpatialSelectionActor : public AActor
{
	GENERATED_BODY()

public:

	AMGSpatialSelectionActor();

	void Initialize(const FVector& StartWorldPos, UMGSpatialSelectionComponent* InOwnerComponent);

	void UpdateBounds(const FVector& CurrentWorldPos);

	FVector GetSelectionBoxExtent() const;

	UBoxComponent* GetSelectionBox() const { return SelectionBox; }

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(BlueprintAssignable)
	FOnSelectionActorFinished OnSelectionFinished;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MG Spatial Selection")
	UBoxComponent* SelectionBox;

	UPROPERTY()
	bool bShowDebug = false;

	UPROPERTY()
	UMGSpatialSelectionComponent* OwnerComponent;

	FVector StartPosition;

	FVector CurrentTargetPos;

	float SelectionHeight = 100.f;
};