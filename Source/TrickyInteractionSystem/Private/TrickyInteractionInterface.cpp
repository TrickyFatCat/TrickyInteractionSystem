// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov


#include "TrickyInteractionInterface.h"


bool ITrickyInteractionInterface::StartInteraction_Implementation(AActor* Interactor)
{
	return false;
}

bool ITrickyInteractionInterface::InterruptInteraction_Implementation(AActor* Interruptor, AActor* Interactor)
{
	return false;
}

bool ITrickyInteractionInterface::FinishInteraction_Implementation(AActor* Interactor)
{
	return false;
}

bool ITrickyInteractionInterface::ForceInteraction_Implementation(AActor* Interactor)
{
	return false;
}