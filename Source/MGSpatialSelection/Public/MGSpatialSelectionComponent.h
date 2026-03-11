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

	void RegisterActor(AActor* Actor);

	void UnregisterActor(AActor* Actor);
protected:

	UFUNCTION()
	void StartSelection();

	void UpdateSelection();

	void MakeRegistration(const TArray<AActor*>& OverlappingActors);

	void DoDecay(float DeltaTime);

	UFUNCTION()
	void FinishSelection();

	bool CanSpawn() const;

	void SpawnSelectionActor(const FVector& StartWorldPos);
	
	void ClearCurrentSelection();

	void SetSelectionState(EMGSelectionState NewState, const FVector& StartLocation = FVector::ZeroVector);

	void DestroySelectionActor();

	bool ShouldRegister(AActor* Actor) const;

public:

	/** Collision channels to check for actors during selection. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Selection Settings")
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels;

	/** The vertical height of the 3D selection volume. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Selection Settings")
	float SelectionHeight = 500.f;

	/** Minimum movement distance (in units) of the mouse required to trigger a visual update. Helps prevent micro-jitter. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Selection Settings")
	float UpdateThreshold = 1.0f;

	/** The collision channel used to trace the ground position under the cursor. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Selection Settings")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	/** Material Parameter Collection to store selection bounds and opacity. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Material Settings")
	TObjectPtr<UMaterialParameterCollection> SelectionMPC;

	/** Name of the Vector parameter in the MPC that stores the selection box center. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Material Settings")
	FString SelectionCenterParameterName = TEXT("SelectionCenter");

	/** Name of the Vector parameter in the MPC that stores the selection box extent. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Material Settings")
	FString SelectionExtentParameterName = TEXT("SelectionExtent");
	
	/** Name of the Scalar parameter in the MPC that stores the current selection opacity. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Material Settings")
	FString SelectionOpacityParameterName = TEXT("SelectionOpacity");
	
	/** The maximum opacity of the selection box while selecting. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Material Settings")
	float SelectionOpacity = 1.f;

	/** Duration (in seconds) it takes for the selection box to fade out after the selection is complete. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Material Settings")
	float DecayTime = 1.f;

	/** Input Action to trigger the selection process. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Input & Debug")
	TObjectPtr<UInputAction> SelectionAction;

	/** If true, draws the selection box using internal debug lines. */
	UPROPERTY(EditAnywhere, Category = "MG Spatial Selection|Input & Debug")
	bool bShowDebug = false;

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
	TSet<AActor*> CurrentSelectedActors;

protected:

	UPROPERTY()
	EMGSelectionState SelectionState = EMGSelectionState::Waiting;

	UPROPERTY()
	AMGSpatialSelectionActor* SelectionActor;

	UPROPERTY()
	bool bSelectionChanged = false;

	UPROPERTY()
	FVector LastUpdatedCenter = FVector::ZeroVector;

	UPROPERTY()
	FVector LastUpdatedExtent = FVector::ZeroVector;

	UPROPERTY()
	float CurrentOpacity = 1.f;

	UPROPERTY()
	bool bIsDecaying = false;

	UPROPERTY()
	APlayerController* CachedPC;
};