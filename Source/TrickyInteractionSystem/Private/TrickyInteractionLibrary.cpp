// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov


#include "TrickyInteractionLibrary.h"

#include "InteractionQueueComponent.h"
#include "TrickyInteractionInterface.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY(LogTrickyInteractionSystem);

bool UTrickyInteractionLibrary::IsActorInteractive(AActor* Actor)
{
	if (!IsValid(Actor) || !Actor->Implements<UTrickyInteractionInterface>())
	{
		return false;
	}

	FInteractionData InteractionData;
	return GetActorInteractionData(Actor, InteractionData);
}

bool UTrickyInteractionLibrary::GetActorInteractionData(AActor* Actor, FInteractionData& InteractionData)
{
	if (!IsValid(Actor) && !Actor->Implements<UTrickyInteractionInterface>())
	{
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		if (!IsValid(Actor))
		{
			const FString ActorName = Actor->GetActorNameOrLabel();
			const FString Instruction = FString::Printf(
				TEXT("Please add UTrickyInteractionInterface to this actor if you want to use it as interactive actor."));
			const FString Message = FString::Printf(
				TEXT("Actor %s does NOT implement UTrickyInteractionInterface.\n%s"), *ActorName, *Instruction);
			PrintError(Message);
		}
		else
		{
			PrintError("Can't get interaction data. Actor is invalid.");
		}
#endif
		return false;
	}

	const FName InteractionDataPropertyName = "InteractionData";
	const UClass* Class = Actor->GetClass();

	FProperty* Property = Class->FindPropertyByName(InteractionDataPropertyName);

	if (!Property)
	{
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		PrintPropertyError(Actor);
#endif
		return false;
	}

	const FStructProperty* StructProperty = CastField<FStructProperty>(Property);

	if (!StructProperty)
	{
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		PrintPropertyError(Actor);
#endif
		return false;
	}

	const FName StructName = StructProperty->Struct->GetFName();

	if (StructName != InteractionDataPropertyName)
	{
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		PrintPropertyError(Actor);
#endif
		return false;
	}

	InteractionData = *StructProperty->ContainerPtrToValuePtr<FInteractionData>(Actor);
	return true;
}

bool UTrickyInteractionLibrary::AddToInteractionQueue(AActor* Interactor, AActor* InteractiveActor)
{
	if (!IsValid(Interactor) || !IsValid(InteractiveActor))
	{
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		if (!IsValid(Interactor))
		{
			PrintWarning("Can't add InteractiveActor to InteractionQueue. Interactor is invalid.");
		}

		if (!IsValid(InteractiveActor))
		{
			PrintWarning("Can't add InteractiveActor to InteractionQueue. InteractiveActor is invalid.");
		}
#endif
		return false;
	}

	UInteractionQueueComponent* InteractionQueueComp = GetInteractionQueueComponent(Interactor);

	if (!IsValid(InteractionQueueComp))
	{
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		const FString InteractorName = Interactor->GetActorNameOrLabel();
		const FString InteractiveActorName = InteractiveActor->GetActorNameOrLabel();
		const FString Message = FString::Printf(
			TEXT("Can't add %s to InteractionQueue of %s. It doesn't have InteractionQueueComponent."),
			*InteractiveActorName, *InteractorName);

		PrintWarning(Message);
#endif
		return false;
	}

	return InteractionQueueComp->AddToInteractionQueue(InteractiveActor);
}

bool UTrickyInteractionLibrary::RemoveFromInteractionQueue(AActor* Interactor, AActor* InteractiveActor)
{
	if (!IsValid(Interactor) || !IsValid(InteractiveActor))
	{
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		if (!IsValid(Interactor))
		{
			PrintWarning("Can't remove InteractiveActor from InteractionQueue. Interactor is invalid.");
		}

		if (!IsValid(InteractiveActor))
		{
			PrintWarning("Can't remove InteractiveActor from InteractionQueue. InteractiveActor is invalid.");
		}
#endif
		return false;
	}

	UInteractionQueueComponent* InteractionQueueComp = GetInteractionQueueComponent(Interactor);

	if (!IsValid(InteractionQueueComp))
	{
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		const FString InteractorName = Interactor->GetActorNameOrLabel();
		const FString InteractiveActorName = InteractiveActor->GetActorNameOrLabel();
		const FString Message = FString::Printf(
			TEXT("Can't remove %s from InteractionQueue of %s. It doesn't have InteractionQueueComponent."),
			*InteractiveActorName, *InteractorName);

		PrintWarning(Message);
#endif
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
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		if (!IsValid(Interactor))
		{
			PrintWarning("Can't check if Actor is in InteractionQueue. Interactor is invalid.");
		}

		if (!IsValid(Actor))
		{
			PrintWarning("Can't check if Actor is in InteractionQueue. Actor is invalid.");
		}
#endif
		return false;
	}

	UInteractionQueueComponent* InteractionQueueComp = GetInteractionQueueComponent(Interactor);

	if (!IsValid(InteractionQueueComp))
	{
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		const FString InteractorName = Interactor->GetActorNameOrLabel();
		const FString ActorName = Actor->GetActorNameOrLabel();
		const FString Message = FString::Printf(
			TEXT("Can't check if %s is in InteractionQueue of %s. It doesn't have InteractionQueueComponent."),
			*ActorName, *InteractorName);

		PrintWarning(Message);
#endif
		return false;
	}

	return InteractionQueueComp->IsInInteractionQueue(Actor);
}

#if WITH_EDITOR && !UE_BUILD_SHIPPING
void UTrickyInteractionLibrary::PrintWarning(const FString& Message)
{
	UE_LOG(LogTrickyInteractionSystem, Warning, TEXT("%s"), *Message);
}

void UTrickyInteractionLibrary::PrintError(const FString& Message)
{
	UE_LOG(LogTrickyInteractionSystem, Error, TEXT("%s"), *Message);
}

void UTrickyInteractionLibrary::PrintPropertyError(const AActor* Actor)
{
	const FString ActorName = Actor->GetActorNameOrLabel();
	const FString Instruction = FString::Printf(
		TEXT("Please add InteractionData variable to this actor if you want to use it as interactive actor."));
	const FString Message = FString::Printf(
		TEXT("Actor %s does NOT have InteractionData property.\n%s"), *ActorName, *Instruction);
	PrintError(Message);
}
#endif
