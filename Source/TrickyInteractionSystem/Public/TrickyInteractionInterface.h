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
	 * @warning This property will be ignored if bRequiresLineOfSight == true
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="InteractionData", meta=(ClampMin=0, UIMin=0))
	int32 InteractionWeight = 0;
};

/**
 * Represents the result of an interaction
 */
UENUM(Blueprintable)
enum class EInteractionResult : uint8
{
	Success,
	Failure,
	Invalid
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
	/**
	 * Starts an interaction sequence by a give actor
	 * @param Interactor The actor which initiated the sequence. Must be a valid actor
	 * @return result of the interaction start
	 */
	UFUNCTION(BlueprintNativeEvent, Category="TrickyInteraction")
	EInteractionResult StartInteraction(AActor* Interactor);

	virtual EInteractionResult StartInteraction_Implementation(AActor* Interactor);

	/**
	 * Interrupts an ongoing interaction sequence
	 * @param Interruptor The actor attempting to interrupt the interaction. Must be a valid actor
	 * @param Interactor The actor currently engaged in the interaction. Must be a valid actor
	 * @return result of the interaction finish
	 */
	UFUNCTION(BlueprintNativeEvent, Category="TrickyInteraction")
	EInteractionResult InterruptInteraction(AActor* Interruptor, AActor* Interactor);

	virtual EInteractionResult InterruptInteraction_Implementation(AActor* Interruptor, AActor* Interactor);

	/**
	 * Finishes an interaction sequence by a given actor
	 * @param Interactor The actor which finishes the interaction. Must be a valid actor
	 * @return result of the interaction interruption
	 */
	UFUNCTION(BlueprintNativeEvent, Category="TrickyInteraction")
	EInteractionResult FinishInteraction(AActor* Interactor);

	virtual EInteractionResult FinishInteraction_Implementation(AActor* Interactor);

	/**
	 * Forces the interaction sequence by a given actor
	 * Usually used for immediate interactions which don't require any animations or time to interact
	 * @param Interactor The actor which forced the interaction. Must be a valid actor
	 * @return of the forced interaction
	 */
	UFUNCTION(BlueprintNativeEvent, Category="TrickyInteraction")
	EInteractionResult ForceInteraction(AActor* Interactor);

	virtual EInteractionResult ForceInteraction_Implementation(AActor* Interactor);
};
