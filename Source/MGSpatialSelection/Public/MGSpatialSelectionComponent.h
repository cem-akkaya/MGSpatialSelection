// MG Spatial Selection System by Cem Akkaya https://www.cemakkaya.com

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputCoreTypes.h"
#include "MGSpatialSelectionInterface.h"
#include "MGSpatialSelectionComponent.generated.h"

class UInputAction;
struct FEnhancedActionInstance;

class AMGSpatialSelectionActor;

UENUM(BlueprintType)
enum class EMGSelectionState : uint8
{
	Waiting,
	Selecting,
	Finished
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionStarted, AActor*, Context);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionFinished, AActor*, Context);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorSelected, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorDeselected, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionUpdated, const TArray<AActor*>&, CurrentSelectedActors);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MGSPATIALSELECTION_API UMGSpatialSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UMGSpatialSelectionComponent();

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	UFUNCTION()
	void StartSelection();

	void UpdateSelection();

	UFUNCTION()
	void FinishSelection();

	bool CanSpawn() const;

	void SpawnSelectionActor(const FVector& StartWorldPos);

	void DestroySelectionActor();

	UFUNCTION()
	void HandleActorEntered(AActor* Actor);

	UFUNCTION()
	void HandleActorLeft(AActor* Actor);

	bool ShouldRegister(AActor* Actor) const;

public:

	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Selection Settings")
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels;

	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Selection Settings")
	float SelectionHeight = 500.f;

	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Selection Settings")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Material Settings")
	TObjectPtr<UMaterialParameterCollection> SelectionMPC;

	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Material Settings")
	FString SelectionCenterParameterName = TEXT("SelectionCenter");

	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Material Settings")
	FString SelectionExtentParameterName = TEXT("SelectionExtent");

	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Input & Debug")
	TObjectPtr<UInputAction> SelectionAction;

	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Input & Debug")
	bool bShowDebug = true;

public:

	UPROPERTY(BlueprintAssignable, Category = "MG Spatial Selection|Events")
	FOnSelectionStarted OnSelectionStarted;

	UPROPERTY(BlueprintAssignable, Category = "MG Spatial Selection|Events")
	FOnSelectionFinished OnSelectionFinished;

	UPROPERTY(BlueprintAssignable, Category = "MG Spatial Selection|Events")
	FOnActorSelected OnActorSelected;

	UPROPERTY(BlueprintAssignable, Category = "MG Spatial Selection|Events")
	FOnActorDeselected OnActorDeselected;

	UPROPERTY(BlueprintAssignable, Category = "MG Spatial Selection|Events")
	FOnSelectionUpdated OnSelectionUpdated;

	UPROPERTY(BlueprintReadOnly, Category = "MG Spatial Selection|State")
	TArray<AActor*> CurrentSelectedActors;


protected:

	UPROPERTY()
	EMGSelectionState SelectionState = EMGSelectionState::Waiting;

	UPROPERTY()
	AMGSpatialSelectionActor* SelectionActor;

	UPROPERTY()
	bool bSelectionChanged = false;

	UPROPERTY()
	APlayerController* CachedPC;
};