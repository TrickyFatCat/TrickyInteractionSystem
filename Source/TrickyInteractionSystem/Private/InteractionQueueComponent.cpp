// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov


#include "InteractionQueueComponent.h"

#include "TrickyInteractionInterface.h"

UInteractionQueueComponent::UInteractionQueueComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 0.05f;
}

void UInteractionQueueComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetComponentTickEnabled(false);
}


void UInteractionQueueComponent::TickComponent(float DeltaTime,
                                               ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


bool UInteractionQueueComponent::AddToInteractionQueue(AActor* InteractiveActor)
{
	if (!IsActorInteractive(InteractiveActor) || IsInInteractionQueue(InteractiveActor))
	{
		return false;
	}

	InteractionQueue.Emplace(InteractiveActor);
	SortInteractionQueue();
	SetComponentTickEnabled(true);
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

	const bool bIsSuccess = ITrickyInteractionInterface::Execute_StartInteraction(InteractiveActor, Interactor);

	if (bIsSuccess)
	{
		FInteractionData InteractionData;
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

	const bool bIsSuccess = ITrickyInteractionInterface::Execute_ForceInteraction(InteractiveActor, Interactor);

	if (bIsSuccess)
	{
		OnInteract.Broadcast(this, InteractiveActor, bIsSuccess);
	}

	return bIsSuccess;
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

bool UInteractionQueueComponent::GetActorInteractionData(const AActor* InteractiveActor, FInteractionData& InteractionData)
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
