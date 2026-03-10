// MG Spatial Selection System by Cem Akkaya https://www.cemakkaya.com

#include "MGSpatialSelectionComponent.h"
#include "MGSpatialSelectionActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

UMGSpatialSelectionComponent::UMGSpatialSelectionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	CollisionChannels.Add(ECC_Pawn);
	CollisionChannels.Add(ECC_WorldDynamic);
	CollisionChannels.Add(ECC_PhysicsBody);

	GridDensity = 100.f;
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
		SelectionActor->OnActorEntered.AddDynamic(this, &UMGSpatialSelectionComponent::HandleActorEntered);
		SelectionActor->OnActorLeft.AddDynamic(this, &UMGSpatialSelectionComponent::HandleActorLeft);
	}
}

void UMGSpatialSelectionComponent::DestroySelectionActor()
{
	if (SelectionActor)
	{
		// Before destroying, notify all currently selected actors that they are no longer selected
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
		OnSelectionUpdated.Broadcast(CurrentSelectedActors);

		SelectionActor->OnActorEntered.RemoveDynamic(this, &UMGSpatialSelectionComponent::HandleActorEntered);
		SelectionActor->OnActorLeft.RemoveDynamic(this, &UMGSpatialSelectionComponent::HandleActorLeft);

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
		SpawnSelectionActor(Hit.Location);
		SelectionState = EMGSelectionState::Selecting;
		OnSelectionStarted.Broadcast(GetOwner());
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

		// Update Surface Points
		SurfacePoints.Empty();
		
		FVector Start = SelectionActor->GetActorLocation();
		FVector Extent = SelectionActor->GetComponentsBoundingBox().GetExtent();
		
		FVector Min = Start - Extent;
		FVector Max = Start + Extent;

		float Step = FMath::Max(GridDensity, 10.f);

		for (float x = Min.X; x <= Max.X; x += Step)
		{
			for (float y = Min.Y; y <= Max.Y; y += Step)
			{
				FVector RayStart(x, y, Max.Z + 100.f);
				FVector RayEnd(x, y, Min.Z - 100.f);

				FHitResult GridHit;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(SelectionActor);
				Params.AddIgnoredActor(GetOwner());

				if (GetWorld()->LineTraceSingleByChannel(GridHit, RayStart, RayEnd, TraceChannel, Params))
				{
					SurfacePoints.Add(GridHit.Location);
					
					if (bShowDebug)
					{
						DrawDebugLine(GetWorld(), RayStart, GridHit.Location, FColor::White, false, -1.0f, 0, 0.5f);
						DrawDebugPoint(GetWorld(), GridHit.Location, 5.0f, FColor::Black, false, -1.0f);
					}
				}
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

	SelectionState = EMGSelectionState::Waiting;
	OnSelectionFinished.Broadcast(GetOwner());
}

void UMGSpatialSelectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SelectionState == EMGSelectionState::Selecting)
	{
		UpdateSelection();

		if (bSelectionChanged)
		{
			OnSelectionUpdated.Broadcast(CurrentSelectedActors);
			bSelectionChanged = false;
		}
	}
}

bool UMGSpatialSelectionComponent::ShouldRegister(AActor* Actor) const
{
	if (!Actor)
		return false;

	if (!Actor->Implements<UMGSpatialSelectionInterface>())
		return false;

	if (CurrentSelectedActors.Contains(Actor))
		return false;

	return true;
}

void UMGSpatialSelectionComponent::HandleActorEntered(AActor* Actor)
{
	if (!ShouldRegister(Actor))
		return;

	CurrentSelectedActors.Add(Actor);

	IMGSpatialSelectionInterface::Execute_OnSelectionStatus(Actor, true);

	OnActorSelected.Broadcast(Actor);
	bSelectionChanged = true;
}

void UMGSpatialSelectionComponent::HandleActorLeft(AActor* Actor)
{
	if (!Actor || !CurrentSelectedActors.Contains(Actor))
		return;

	CurrentSelectedActors.Remove(Actor);

	IMGSpatialSelectionInterface::Execute_OnSelectionStatus(Actor, false);

	OnActorDeselected.Broadcast(Actor);
	bSelectionChanged = true;
}


bool UMGSpatialSelectionComponent::CanSpawn() const
{
	if (SelectionActor != nullptr)
		return false;

	return true;
}
