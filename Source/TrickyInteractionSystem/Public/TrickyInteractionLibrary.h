// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TrickyInteractionLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTrickyInteractionSystem, Log, All)

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
	static bool IsActorInteractive(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category="TrickyInteraction", meta=(WorldContext="Actor"))
	static bool GetActorInteractionData(AActor* Actor,
	                                    FInteractionData& InteractionData);

	UFUNCTION(BlueprintCallable, Category="TrickyInteraction", meta=(WorldContext="Actor"))
	static bool AddToInteractionQueue(AActor* Interactor, AActor* InteractiveActor);

	UFUNCTION(BlueprintCallable, Category="TrickyInteraction", meta=(WorldContext="Actor"))
	static bool RemoveFromInteractionQueue(AActor* Interactor, AActor* InteractiveActor);

	UFUNCTION(BlueprintCallable, Category="TrickyInteraction", meta=(WorldContext="Actor"))
	static UInteractionQueueComponent* GetInteractionQueueComponent(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category="TrickyInteraction", meta=(WorldContext="Actor"))
	static bool IsInInteractionQueue(const AActor* Interactor, AActor* Actor);

private:
#if WITH_EDITOR && !UE_BUILD_SHIPPING
	static void PrintWarning(const FString& Message);

	static void PrintError(const FString& Message);

	static void PrintPropertyError(const AActor* Actor);
#endif
};
