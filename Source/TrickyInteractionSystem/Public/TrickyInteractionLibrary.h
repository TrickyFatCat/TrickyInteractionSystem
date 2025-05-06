// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TrickyInteractionLibrary.generated.h"

struct FInteractionData;
/**
 * 
 */
UCLASS()
class TRICKYINTERACTIONSYSTEM_API UTrickyInteractionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="TrickyInteraction", meta=(WorldContext="Actor"))
	static bool IsActorInteractive(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category="TrickyInteraction", meta=(WorldContext="Actor"))
	static bool GetActorInteractionData(const AActor* Actor,
	                                    FInteractionData& InteractionData);

private:
	const FString InteractionDataName = "InteractionData";
};
