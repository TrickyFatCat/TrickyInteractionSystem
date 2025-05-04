// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov


#include "InteractionQueueComponent.h"

#include "TrickyInteractionInterface.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UInteractionQueueComponent::UInteractionQueueComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UInteractionQueueComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetComponentTickEnabled(false);
	ActorsToIgnore.AddUnique(GetOwner());
}


void UInteractionQueueComponent::TickComponent(float DeltaTime,
                                               ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult HitResult;
	CheckLineOfSight(DeltaTime, HitResult);

	if (HitResult.bBlockingHit)
	{
		const FVector TraceDirection = UKismetMathLibrary::GetDirectionUnitVector(
			GetOwner()->GetActorLocation(), HitResult.TraceEnd);
		const FVector ImpactDirection = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(),
			HitResult.Location);
		const float DotProduct = FVector::DotProduct(TraceDirection, ImpactDirection);
		
		ActorInSight = DotProduct < 0.f ? nullptr : HitResult.GetActor();

		if (IsActorInteractive(ActorInSight) && IsInInteractionQueue(ActorInSight))
		{
			FInteractionData InteractionData;
			GetActorInteractionData(ActorInSight, InteractionData);

			if (InteractionData.bRequiresLineOfSight)
			{
				const int32 Index = InteractionQueue.IndexOfByKey(ActorInSight);
				InteractionQueue.Swap(Index, 0);
			}
		}
	}
}


bool UInteractionQueueComponent::AddToInteractionQueue(AActor* InteractiveActor)
{
	if (!IsActorInteractive(InteractiveActor) || IsInInteractionQueue(InteractiveActor))
	{
		return false;
	}

	InteractionQueue.Emplace(InteractiveActor);
	SortInteractionQueue();

	if (bUseLineOfSight && !IsComponentTickEnabled())
	{
		ToggleComponentTick();
	}
	
	OnActorAddedToInteractionQueue.Broadcast(this, InteractiveActor);
	return true;
}

bool UInteractionQueueComponent::RemoveFromInteractionQueue(AActor* InteractiveActor)
{
	if (!IsActorInteractive(InteractiveActor) || !IsInInteractionQueue(InteractiveActor) || IsInteractionQueueEmpty())
	{
		return false;
	}

	if (InteractionQueue.RemoveSingle(InteractiveActor) <= 0)
	{
		return false;
	}

	SortInteractionQueue();
	OnActorRemovedFromInteractionQueue.Broadcast(this, InteractiveActor);

	if (IsInteractionQueueEmpty())
	{
		SetComponentTickEnabled(false);
	}

	return true;
}


bool UInteractionQueueComponent::IsInInteractionQueue(AActor* InteractiveActor)
{
	if (!IsActorInteractive(InteractiveActor))
	{
		return false;
	}

	return InteractionQueue.Contains(InteractiveActor);
}

void UInteractionQueueComponent::SetUseLineOfSight(bool Value)
{
	if (bUseLineOfSight == Value)
	{
		return;
	}

	bUseLineOfSight = Value;
}

bool UInteractionQueueComponent::StartInteraction()
{
	AActor* Interactor = GetOwner();

	if (IsInteractionQueueEmpty() || !IsValid(Interactor))
	{
		return false;
	}

	AActor* InteractiveActor = InteractionQueue[0];

	if (!IsValid(InteractiveActor))
	{
		return false;
	}

	FInteractionData InteractionData;
	GetActorInteractionData(InteractiveActor, InteractionData);

	if (InteractionData.bRequiresLineOfSight && InteractiveActor != ActorInSight)
	{
		return false;
	}

	const bool bIsSuccess = ITrickyInteractionInterface::Execute_StartInteraction(InteractiveActor, Interactor);

	if (bIsSuccess)
	{
		GetActorInteractionData(InteractiveActor, InteractionData);
		OnInteractionStarted.Broadcast(this, InteractiveActor, InteractionData);
	}

	return bIsSuccess;
}

bool UInteractionQueueComponent::FinishInteraction()
{
	AActor* Interactor = GetOwner();

	if (IsInteractionQueueEmpty() || !IsValid(Interactor))
	{
		return false;
	}

	AActor* InteractiveActor = InteractionQueue[0];

	if (!IsValid(InteractiveActor))
	{
		return false;
	}

	const bool bIsSuccess = ITrickyInteractionInterface::Execute_FinishInteraction(InteractiveActor, Interactor);
	OnInteractionFinished.Broadcast(this, InteractiveActor, bIsSuccess);
	return bIsSuccess;
}

bool UInteractionQueueComponent::InterruptInteraction(AActor* Interruptor)
{
	AActor* Interactor = GetOwner();

	if (IsInteractionQueueEmpty() || !IsValid(Interactor))
	{
		return false;
	}

	AActor* InteractiveActor = InteractionQueue[0];

	if (!IsValid(InteractiveActor))
	{
		return false;
	}

	const bool bIsSuccess = ITrickyInteractionInterface::Execute_InterruptInteraction(InteractiveActor,
		Interruptor,
		Interactor);

	if (bIsSuccess)
	{
		OnInteractionInterrupted.Broadcast(this, InteractiveActor, Interruptor);
	}

	return bIsSuccess;
}

bool UInteractionQueueComponent::ForceInteraction()
{
	AActor* Interactor = GetOwner();

	if (IsInteractionQueueEmpty() || !IsValid(Interactor))
	{
		return false;
	}

	AActor* InteractiveActor = InteractionQueue[0];

	if (!IsValid(InteractiveActor))
	{
		return false;
	}

	FInteractionData InteractionData;
	GetActorInteractionData(InteractiveActor, InteractionData);

	if (InteractionData.bRequiresLineOfSight && InteractiveActor != ActorInSight)
	{
		return false;
	}

	const bool bIsSuccess = ITrickyInteractionInterface::Execute_ForceInteraction(InteractiveActor, Interactor);

	if (bIsSuccess)
	{
		OnInteract.Broadcast(this, InteractiveActor, bIsSuccess);
	}

	return bIsSuccess;
}

void UInteractionQueueComponent::RegisterCamera(UCameraComponent* Camera)
{
	if (!IsValid(Camera))
	{
		return;
	}

	CameraComponent = Camera;
	ActorsToIgnore.AddUnique(CameraComponent->GetOwner());
}

bool UInteractionQueueComponent::IsActorInteractive(const AActor* Actor)
{
	if (!IsValid(Actor) || !Actor->Implements<UTrickyInteractionInterface>())
	{
		return false;
	}

	FInteractionData InteractionData;
	return ITrickyInteractionInterface::Execute_GetInteractionData(Actor, InteractionData);
}

bool UInteractionQueueComponent::GetActorInteractionData(const AActor* InteractiveActor,
                                                         FInteractionData& InteractionData)
{
	if (!IsValid(InteractiveActor) || !InteractiveActor->Implements<UTrickyInteractionInterface>())
	{
		return false;
	}

	return ITrickyInteractionInterface::Execute_GetInteractionData(InteractiveActor, InteractionData);
}

void UInteractionQueueComponent::SortInteractionQueue()
{
	if (InteractionQueue.Num() <= 1)
	{
		return;
	}

	auto Predicate = [](const AActor* ActorA, const AActor* ActorB) -> bool
	{
		FInteractionData InteractionDataA;
		GetActorInteractionData(ActorA, InteractionDataA);
		FInteractionData InteractionDataB;
		GetActorInteractionData(ActorB, InteractionDataB);
		return InteractionDataA.InteractionWeight >= InteractionDataB.InteractionWeight;
	};

	Algo::Sort(InteractionQueue, Predicate);
}

void UInteractionQueueComponent::ToggleComponentTick()
{
	if (!IsValid(CameraComponent))
	{
		return;
	}

	SetComponentTickEnabled(bUseLineOfSight && !IsInteractionQueueEmpty() && !IsComponentTickEnabled());
}

void UInteractionQueueComponent::CheckLineOfSight(const float DeltaTime, FHitResult& OutHitResult)
{
	if (!IsValid(CameraComponent))
	{
		return;
	}

	FMinimalViewInfo ViewInfo;
	CameraComponent->GetCameraView(DeltaTime, ViewInfo);

	const FVector StartPoint = ViewInfo.Location;
	const FVector EndPoint = ViewInfo.Location + ViewInfo.Rotation.Vector() * LineOfSightDistance;

	UKismetSystemLibrary::SphereTraceSingle(GetOwner(),
	                                        StartPoint,
	                                        EndPoint,
	                                        LineOfSightRadius,
	                                        TraceChannel,
	                                        false,
	                                        ActorsToIgnore,
	                                        DrawDebugType,
	                                        OutHitResult,
	                                        true,
	                                        TraceColor,
	                                        TraceHitColor,
	                                        DrawTime);
}
