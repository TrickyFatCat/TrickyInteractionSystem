// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov


#include "TrickyInteractionLibrary.h"

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
