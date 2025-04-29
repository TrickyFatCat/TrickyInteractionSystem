// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TrickyInteractionInterface.generated.h"

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="InteractionData")
	FText InteractionMessage = FText::FromString("Interact");
	
	UPROPERTY(BlueprintReadWrite, Category="InteractionData", meta=(ClampMin=0, UIMin=0, ClampMax=99, UIMax=99))
	float InteractionTime = 0.f;

	UPROPERTY(BlueprintReadWrite, Category="InteractionData")
	bool bRequiresLineOfSight = false;

	UPROPERTY(BlueprintReadWrite, Category="InteractionData", meta=(ClampMin=0, UIMin=0))
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
	bool InterruptInteraction(AActor* Interruptor);

	virtual bool InterruptInteraction_Implementation(AActor* Interruptor);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="TrickyInteraction")
	bool FinishInteraction(AActor* Interactor);

	virtual bool FinishInteraction_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="TrickyInteraction")
	bool ForceInteraction(AActor* Interactor);

	virtual bool ForceInteraction_Implementation(AActor* Interactor);

	UFUNCTION(BuleprintPure, BlueprintImplementableEvent, Category="TrickyInteraction")
	bool GetInteractionData(FInteractionData& InteractionData);

	virtual bool GetInteractionData_Implementation(FInteractionData& InteractionData);
};
