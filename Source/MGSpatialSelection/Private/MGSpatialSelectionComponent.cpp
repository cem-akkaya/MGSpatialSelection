// MG Spatial Selection System by Cem Akkaya https://www.cemakkaya.com

#include "MGSpatialSelectionComponent.h"
#include "MGSpatialSelectionActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"

UMGSpatialSelectionComponent::UMGSpatialSelectionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	CollisionChannels.Add(ECC_Pawn);
	CollisionChannels.Add(ECC_WorldDynamic);
	CollisionChannels.Add(ECC_PhysicsBody);

	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> DefaultMPC(TEXT("/MGSpatialSelection/MGSpatialSelection_MPC.MGSpatialSelection_MPC"));
	if (DefaultMPC.Succeeded())
	{
		SelectionMPC = DefaultMPC.Object;
	}
}

void UMGSpatialSelectionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		if (APawn* PawnOwner = Cast<APawn>(Owner))
		{
			CachedPC = Cast<APlayerController>(PawnOwner->GetController());
		}
		else if (APlayerController* PCOwner = Cast<APlayerController>(Owner))
		{
			CachedPC = PCOwner;
		}
	}

	if (!CachedPC)
	{
		CachedPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}

	// Setup Enhanced Input
	if (CachedPC)
	{
		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(CachedPC->InputComponent))
		{
			if (SelectionAction)
			{
				EIC->BindAction(SelectionAction, ETriggerEvent::Started, this, &UMGSpatialSelectionComponent::StartSelection);
				EIC->BindAction(SelectionAction, ETriggerEvent::Completed, this, &UMGSpatialSelectionComponent::FinishSelection);
				EIC->BindAction(SelectionAction, ETriggerEvent::Canceled, this, &UMGSpatialSelectionComponent::FinishSelection);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[MG Spatial Selection] SelectionAction is not assigned. Please assign it in the component settings."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[MG Spatial Selection] Cannot find Enhanced Input Component on Player Controller."));
		}
	}
}

void UMGSpatialSelectionComponent::SpawnSelectionActor(const FVector& StartWorldPos)
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	SelectionActor = World->SpawnActor<AMGSpatialSelectionActor>(
		AMGSpatialSelectionActor::StaticClass(),
		StartWorldPos,
		FRotator::ZeroRotator
	);

	if (SelectionActor)
	{
		SelectionActor->Initialize(StartWorldPos, this);
	}
}

void UMGSpatialSelectionComponent::ClearCurrentSelection()
{
	if (CurrentSelectedActors.Num() > 0)
	{
		for (AActor* Actor : CurrentSelectedActors)
		{
			if (Actor)
			{
				if (Actor->Implements<UMGSpatialSelectionInterface>())
				{
					IMGSpatialSelectionInterface::Execute_OnSelectionStatus(Actor, false);
				}
				OnActorDeselected.Broadcast(Actor);
			}
		}

		CurrentSelectedActors.Empty();
		OnSelectionUpdated.Broadcast(CurrentSelectedActors.Array());
	}
}

void UMGSpatialSelectionComponent::SetSelectionState(EMGSelectionState NewState, const FVector& StartLocation)
{
	SelectionState = NewState;

	switch (SelectionState)
	{
	case EMGSelectionState::Selecting:
		ClearCurrentSelection();
		SpawnSelectionActor(StartLocation);

		CurrentOpacity = SelectionOpacity;
		bIsDecaying = false;
		OnSelectionStarted.Broadcast(GetOwner());
		break;

	case EMGSelectionState::Finished:
	case EMGSelectionState::Waiting:
		bIsDecaying = (DecayTime > 0.f);
		if (!bIsDecaying)
		{
			CurrentOpacity = 0.f;
			if (SelectionMPC)
			{
				UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), SelectionMPC, FName(*SelectionOpacityParameterName), CurrentOpacity);
			}
		}
		OnSelectionFinished.Broadcast(GetOwner());
		break;
	}
}

void UMGSpatialSelectionComponent::DestroySelectionActor()
{
	if (SelectionActor)
	{
		ClearCurrentSelection();

		SelectionActor->Destroy();
		SelectionActor = nullptr;
	}
}

void UMGSpatialSelectionComponent::StartSelection()
{
	if (!CanSpawn() || !CachedPC)
		return;

	FHitResult Hit;
	if (CachedPC->GetHitResultUnderCursorByChannel(StaticCast<ETraceTypeQuery>(TraceChannel.GetValue()), true, Hit))
	{
		if (bShowDebug)
		{
			DrawDebugSphere(GetWorld(), Hit.Location, 10.0f, 12, FColor::Red, false, 1.0f);
		}
		
		SetSelectionState(EMGSelectionState::Selecting, Hit.Location);
	}
}

void UMGSpatialSelectionComponent::UpdateSelection()
{
	if (SelectionState != EMGSelectionState::Selecting || !SelectionActor || !CachedPC)
		return;

	FHitResult Hit;
	if (CachedPC->GetHitResultUnderCursorByChannel(StaticCast<ETraceTypeQuery>(TraceChannel.GetValue()), true, Hit))
	{
		if (bShowDebug)
		{
			DrawDebugLine(GetWorld(), SelectionActor->GetActorLocation(), Hit.Location, FColor::Yellow, false, -1.0f, 0, 1.0f);
		}
		SelectionActor->UpdateBounds(Hit.Location);

		// Update Material Parameter Collection
		if (SelectionMPC)
		{
			FVector Center = SelectionActor->GetActorLocation();
			FVector Extent = SelectionActor->GetSelectionBoxExtent();

			bool bCenterChanged = FVector::DistSquared(Center, LastUpdatedCenter) > FMath::Square(UpdateThreshold);
			bool bExtentChanged = FVector::DistSquared(Extent, LastUpdatedExtent) > FMath::Square(UpdateThreshold);

			if (bCenterChanged || bExtentChanged)
			{
				FLinearColor CenterRounded(Center.X, Center.Y, Center.Z);
				FLinearColor ExtentRounded(Extent.X, Extent.Y, Extent.Z);

				UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), SelectionMPC, FName(*SelectionCenterParameterName), CenterRounded);
				UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), SelectionMPC, FName(*SelectionExtentParameterName), ExtentRounded);

				UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), SelectionMPC, FName(*SelectionOpacityParameterName), CurrentOpacity);

				LastUpdatedCenter = Center;
				LastUpdatedExtent = Extent;
			}
		}
	}
}

void UMGSpatialSelectionComponent::FinishSelection()
{
	if (SelectionActor)
	{
		DestroySelectionActor();
	}

	SetSelectionState(EMGSelectionState::Finished);
}

void UMGSpatialSelectionComponent::RegisterActor(AActor* Actor)
{
	if (ShouldRegister(Actor) && !CurrentSelectedActors.Contains(Actor))
	{
		CurrentSelectedActors.Add(Actor);
		IMGSpatialSelectionInterface::Execute_OnSelectionStatus(Actor, true);
		OnActorSelected.Broadcast(Actor);
		OnSelectionUpdated.Broadcast(CurrentSelectedActors.Array());
	}
}

void UMGSpatialSelectionComponent::UnregisterActor(AActor* Actor)
{
	if (Actor && CurrentSelectedActors.Contains(Actor))
	{
		CurrentSelectedActors.Remove(Actor);
		IMGSpatialSelectionInterface::Execute_OnSelectionStatus(Actor, false);
		OnActorDeselected.Broadcast(Actor);
		OnSelectionUpdated.Broadcast(CurrentSelectedActors.Array());
	}
}

void UMGSpatialSelectionComponent::MakeRegistration(const TArray<AActor*>& OverlappingActors)
{
	TSet<AActor*> NewSelection;
	for (AActor* Actor : OverlappingActors)
	{
		if (ShouldRegister(Actor))
		{
			NewSelection.Add(Actor);
		}
	}

	if (CurrentSelectedActors.Num() == NewSelection.Num() && CurrentSelectedActors.Includes(NewSelection))
	{
		return;
	}

	// Identify Added
	for (AActor* Actor : NewSelection)
	{
		if (!CurrentSelectedActors.Contains(Actor))
		{
			IMGSpatialSelectionInterface::Execute_OnSelectionStatus(Actor, true);
			OnActorSelected.Broadcast(Actor);
		}
	}

	// Identify Removed
	for (AActor* Actor : CurrentSelectedActors)
	{
		if (!NewSelection.Contains(Actor))
		{
			IMGSpatialSelectionInterface::Execute_OnSelectionStatus(Actor, false);
			OnActorDeselected.Broadcast(Actor);
		}
	}

	CurrentSelectedActors = MoveTemp(NewSelection);
	OnSelectionUpdated.Broadcast(CurrentSelectedActors.Array());
}

void UMGSpatialSelectionComponent::DoDecay(float DeltaTime)
{
	if (DecayTime > 0.f)
	{
		CurrentOpacity -= (SelectionOpacity / DecayTime) * DeltaTime;
		if (CurrentOpacity <= 0.f)
		{
			CurrentOpacity = 0.f;
			bIsDecaying = false;
		}
	}
	else
	{
		CurrentOpacity = 0.f;
		bIsDecaying = false;
	}

	if (SelectionMPC)
	{
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), SelectionMPC, FName(*SelectionOpacityParameterName), CurrentOpacity);
	}
}

void UMGSpatialSelectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SelectionState == EMGSelectionState::Selecting)
	{
		UpdateSelection();
	}
	else if (bIsDecaying)
	{
		DoDecay(DeltaTime);
	}
}

bool UMGSpatialSelectionComponent::ShouldRegister(AActor* Actor) const
{
	if (!Actor)
		return false;

	if (!Actor->Implements<UMGSpatialSelectionInterface>())
		return false;

	return true;
}

bool UMGSpatialSelectionComponent::CanSpawn() const
{
	if (SelectionActor != nullptr)
		return false;

	return true;
}
