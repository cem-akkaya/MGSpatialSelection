// MG Spatial Selection System by Cem Akkaya https://www.cemakkaya.com

#include "MGSpatialSelectionActor.h"
#include "MGSpatialSelectionComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AMGSpatialSelectionActor::AMGSpatialSelectionActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SelectionBox = CreateDefaultSubobject<UBoxComponent>("SelectionBox");
	RootComponent = SelectionBox;

	SelectionBox->SetGenerateOverlapEvents(true);
	SelectionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


void AMGSpatialSelectionActor::Initialize(const FVector& StartWorldPos, UMGSpatialSelectionComponent* InOwnerComponent)
{
	StartPosition = StartWorldPos;
	CurrentTargetPos = StartWorldPos;
	OwnerComponent = InOwnerComponent;

	if (OwnerComponent)
	{
		SelectionHeight = OwnerComponent->SelectionHeight;
		bShowDebug = OwnerComponent->bShowDebug;

		if (SelectionBox)
		{
			SelectionBox->OnComponentBeginOverlap.AddDynamic(this, &AMGSpatialSelectionActor::OnOverlapBegin);
			SelectionBox->OnComponentEndOverlap.AddDynamic(this, &AMGSpatialSelectionActor::OnOverlapEnd);

			SelectionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
			SelectionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			SelectionBox->SetGenerateOverlapEvents(true);

			if (OwnerComponent->CollisionChannels.Num() > 0)
			{
				SelectionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
				for (ECollisionChannel Channel : OwnerComponent->CollisionChannels)
				{
					SelectionBox->SetCollisionResponseToChannel(Channel, ECR_Overlap);
				}
			}
		}
	}
}

void AMGSpatialSelectionActor::UpdateBounds(const FVector& CurrentWorldPos)
{
	CurrentTargetPos = CurrentWorldPos;
	
	FVector A = StartPosition;
	FVector B = CurrentTargetPos;

	FVector Min;
	FVector Max;

	Min.X = FMath::Min(A.X, B.X);
	Min.Y = FMath::Min(A.Y, B.Y);
	Min.Z = A.Z - (SelectionHeight * 0.5f);

	Max.X = FMath::Max(A.X, B.X);
	Max.Y = FMath::Max(A.Y, B.Y);
	Max.Z = A.Z + (SelectionHeight * 0.5f);

	FVector Center = (Min + Max) * 0.5f;
	FVector Extent = (Max - Min) * 0.5f;

	if (SelectionBox)
	{
		SetActorLocation(Center, false, nullptr, ETeleportType::None);
		SelectionBox->SetBoxExtent(Extent, true);
	}

	if (bShowDebug)
	{
		DrawDebugBox(GetWorld(), Center, Extent, FColor::Green, false, -1.0f, 0, 2.0f);
	}
}

FVector AMGSpatialSelectionActor::GetSelectionBoxExtent() const
{
	return SelectionBox ? SelectionBox->GetUnscaledBoxExtent() : FVector::ZeroVector;
}


void AMGSpatialSelectionActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OwnerComponent != nullptr && OtherActor != nullptr && OtherActor != this)
	{
		OwnerComponent->RegisterActor(OtherActor);
	}
}

void AMGSpatialSelectionActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OwnerComponent != nullptr && OtherActor != nullptr && OtherActor != this)
	{
		OwnerComponent->UnregisterActor(OtherActor);
	}
}