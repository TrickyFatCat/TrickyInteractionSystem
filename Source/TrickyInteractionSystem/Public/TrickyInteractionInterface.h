// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TrickyInteractionInterface.generated.h"

USTRUCT(Blueprintable)
struct FInteractionData
{
	GENERATED_BODY()

	/**
	 * Holds the text that will be shown to the player as a prompt or description
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="InteractionData")
	FText InteractionMessage = FText::FromString("Interact");

	/**
	 * If true, the interaction will only be allowed if the interactive actor is in the line of sight
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="InteractionData")
	bool bRequiresLineOfSight = false;

	/**
	 * Determines the interaction weight, which is used to sort the interaction queue
	 * The higher the value, the higher the priority is.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="InteractionData", meta=(ClampMin=0, UIMin=0))
	int32 InteractionWeight = 0;
};


// This class does not need to be modified.
UINTERFACE()
class UTrickyInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TRICKYINTERACTIONSYSTEM_API ITrickyInteractionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="TrickyInteraction")
	bool StartInteraction(AActor* Interactor);

	virtual bool StartInteraction_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="TrickyInteraction")
	bool InterruptInteraction(AActor* Interruptor, AActor* Interactor);

	virtual bool InterruptInteraction_Implementation(AActor* Interruptor, AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="TrickyInteraction")
	bool FinishInteraction(AActor* Interactor);

	virtual bool FinishInteraction_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="TrickyInteraction")
	bool ForceInteraction(AActor* Interactor);

	virtual bool ForceInteraction_Implementation(AActor* Interactor);
};
