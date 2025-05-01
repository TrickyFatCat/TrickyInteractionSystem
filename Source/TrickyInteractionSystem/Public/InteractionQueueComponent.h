// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionQueueComponent.generated.h"

struct FInteractionData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorAddedToInteractionQueueDynamicSignature,
                                             UInteractionQueueComponent*, Component,
                                             AActor*, InteractiveActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorRemovedFromInteractionQueueDynamicSignature,
                                             UInteractionQueueComponent*, Component,
                                             AActor*, InteractiveActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionStartedDynamicSignature,
                                               UInteractionQueueComponent*, Component,
                                               AActor*, InteractiveActor,
                                               const FInteractionData&, InteractionData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionFinishedDynamicSignature,
                                               UInteractionQueueComponent*, Component,
                                               AActor*, InteractievActor,
                                               bool, bIsSuccessful);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionInterruptedDynamicSignature,
                                               UInteractionQueueComponent*, Component,
                                               AActor*, InteractiveActor,
                                               AActor*, Interruptor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractDynamicSignature,
                                               UInteractionQueueComponent*, Component,
                                               AActor*, InteractiveActor,
                                               bool, bIsSuccessful);

UCLASS(ClassGroup=(TrickyInteractionSystem), meta=(BlueprintSpawnableComponent))
class TRICKYINTERACTIONSYSTEM_API UInteractionQueueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionQueueComponent();

protected:
	virtual void InitializeComponent() override;

public:
	virtual void TickComponent(float DeltaTime,
	                           ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnActorAddedToInteractionQueueDynamicSignature OnActorAddedToInteractionQueue;

	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnActorRemovedFromInteractionQueueDynamicSignature OnActorRemovedFromInteractionQueue;

	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnInteractionStartedDynamicSignature OnInteractionStarted;

	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnInteractionFinishedDynamicSignature OnInteractionFinished;

	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnInteractionInterruptedDynamicSignature OnInteractionInterrupted;

	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnInteractDynamicSignature OnInteract;

	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	bool AddToInteractionQueue(AActor* InteractiveActor);

	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	bool RemoveFromInteractionQueue(AActor* InteractiveActor);

	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	bool IsInInteractionQueue(AActor* InteractiveActor);

	UFUNCTION(BlueprintGetter, Category="InteractionQueue")
	TArray<AActor*> GetInteractionQueue() const
	{
		return InteractionQueue;
	};

	UFUNCTION(BlueprintGetter, Category="InteractionQueue")
	bool GetUseLineOfSight() const { return bUseLineOfSight; };

	UFUNCTION(BlueprintSetter, Category="InteractionQueue")
	void SetUseLineOfSight(bool Value);

	UFUNCTION(BlueprintPure, Category="InteractionQueue")
	bool IsInteractionQueueEmpty() const { return InteractionQueue.IsEmpty(); };

	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	bool StartInteraction();

	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	bool FinishInteraction();

	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	bool InterruptInteraction(AActor* Interruptor);

	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	bool ForceInteraction();

private:
	UPROPERTY(EditDefaultsOnly,
		BlueprintGetter=GetUseLineOfSight,
		BlueprintSetter=SetUseLineOfSight,
		Category="InteractionQueue")
	bool bUseLineOfSight = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetInteractionQueue, Category="InteractionQueue")
	TArray<AActor*> InteractionQueue;

	static bool IsActorInteractive(const AActor* Actor);

	static bool GetActorInteractionData(const AActor* InteractiveActor, FInteractionData& InteractionData);

	void SortInteractionQueue();
};
