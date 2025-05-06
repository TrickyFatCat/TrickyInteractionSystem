// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov


#include "TrickyInteractionLibrary.h"

#include "InteractionQueueComponent.h"
#include "TrickyInteractionInterface.h"

bool UTrickyInteractionLibrary::IsActorInteractive(const AActor* Actor)
{
	if (!IsValid(Actor) || !Actor->Implements<UTrickyInteractionInterface>())
	{
		return false;
	}

	FInteractionData InteractionData;
	return GetActorInteractionData(Actor, InteractionData);
}

bool UTrickyInteractionLibrary::GetActorInteractionData(const AActor* Actor, FInteractionData& InteractionData)
{
	if (!IsValid(Actor) && !Actor->Implements<UTrickyInteractionInterface>())
	{
		return false;
	}

	const UClass* Class = Actor->GetClass();

	FProperty* Property = Class->FindPropertyByName(TEXT("InteractionData"));

	if (!Property)
	{
		return false;
	}

	const FStructProperty* StructProperty = CastField<FStructProperty>(Property);

	if (!StructProperty)
	{
		return false;
	}

	const FName StructName = StructProperty->Struct->GetFName();

	if (StructName != FName("InteractionData"))
	{
		return false;
	}

	InteractionData = *StructProperty->ContainerPtrToValuePtr<FInteractionData>(Actor);
	return true;
}

bool UTrickyInteractionLibrary::AddToInteractionQueue(AActor* Interactor, AActor* InteractiveActor)
{
	if (!IsValid(Interactor) || !IsValid(InteractiveActor))
	{
		return false;
	}

	UInteractionQueueComponent* InteractionQueueComp = GetInteractionQueueComponent(Interactor);

	if (!IsValid(InteractionQueueComp))
	{
		return false;
	}

	return InteractionQueueComp->AddToInteractionQueue(InteractiveActor);
}

bool UTrickyInteractionLibrary::RemoveFromInteractionQueue(AActor* Interactor, AActor* InteractiveActor)
{
	if (!IsValid(Interactor) || !IsValid(InteractiveActor))
	{
		return false;
	}

	UInteractionQueueComponent* InteractionQueueComp = GetInteractionQueueComponent(Interactor);

	if (!IsValid(InteractionQueueComp))
	{
		return false;
	}

	return InteractionQueueComp->RemoveFromInteractionQueue(InteractiveActor);
}

UInteractionQueueComponent* UTrickyInteractionLibrary::GetInteractionQueueComponent(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return nullptr;
	}

	return Actor->FindComponentByClass<UInteractionQueueComponent>();
}

bool UTrickyInteractionLibrary::IsInInteractionQueue(const AActor* Interactor, AActor* Actor)
{
	if (!IsValid(Interactor) || !IsValid(Actor))
	{
		return false;
	}

	UInteractionQueueComponent* InteractionQueueComp = GetInteractionQueueComponent(Interactor);

	if (!IsValid(InteractionQueueComp))
	{
		return false;
	}

	return InteractionQueueComp->IsInInteractionQueue(Actor);
}
