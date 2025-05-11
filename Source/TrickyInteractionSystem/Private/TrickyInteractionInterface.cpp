// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov


#include "TrickyInteractionInterface.h"


EInteractionResult ITrickyInteractionInterface::StartInteraction_Implementation(AActor* Interactor)
{
	return EInteractionResult::Invalid;
}

EInteractionResult ITrickyInteractionInterface::InterruptInteraction_Implementation(AActor* Interruptor, AActor* Interactor)
{
	return EInteractionResult::Invalid;
}

EInteractionResult ITrickyInteractionInterface::FinishInteraction_Implementation(AActor* Interactor)
{
	return EInteractionResult::Invalid;
}

EInteractionResult ITrickyInteractionInterface::ForceInteraction_Implementation(AActor* Interactor)
{
	return EInteractionResult::Invalid;
}