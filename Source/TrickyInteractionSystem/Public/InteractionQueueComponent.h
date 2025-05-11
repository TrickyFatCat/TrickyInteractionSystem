// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractionQueueComponent.generated.h"

namespace EDrawDebugTrace
{
	enum Type : int;
}

class UCameraComponent;
struct FInteractionData;
enum class EInteractionResult : uint8;

DECLARE_LOG_CATEGORY_EXTERN(LogInteractionQueueComponent, Log, All)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorAddedToInteractionQueueDynamicSignature,
                                             UInteractionQueueComponent*, Component,
                                             AActor*, InteractiveActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorRemovedFromInteractionQueueDynamicSignature,
                                             UInteractionQueueComponent*, Component,
                                             AActor*, InteractiveActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionStartedDynamicSignature,
                                               UInteractionQueueComponent*, Component,
                                               AActor*, InteractiveActor,
                                               EInteractionResult, InteractionResult);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionFinishedDynamicSignature,
                                               UInteractionQueueComponent*, Component,
                                               AActor*, InteractievActor,
                                               EInteractionResult, InteractionResult);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnInteractionInterruptedDynamicSignature,
                                               UInteractionQueueComponent*, Component,
                                               AActor*, InteractiveActor,
                                               AActor*, Interruptor,
                                               EInteractionResult, InteractionResult);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionForcedDynamicSignature,
                                               UInteractionQueueComponent*, Component,
                                               AActor*, InteractiveActor,
                                               EInteractionResult, InteractionResult);

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

	/**
	 * Called when a new interactive actor is added to the interaction queue
	 */
	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnActorAddedToInteractionQueueDynamicSignature OnActorAddedToInteractionQueue;

	/**
	 * Called when an interactive actor is removed from the interaction queue
	 */
	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnActorRemovedFromInteractionQueueDynamicSignature OnActorRemovedFromInteractionQueue;

	/**
	 * Called when interaction is started
	 */
	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnInteractionStartedDynamicSignature OnInteractionStarted;

	/**
	 * Called when interaction is finished
	 */
	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnInteractionFinishedDynamicSignature OnInteractionFinished;

	/**
	 * Called when interaction is interrupted
	 */
	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnInteractionInterruptedDynamicSignature OnInteractionInterrupted;

	/**
	 * Called when interaction is forced
	 */
	UPROPERTY(BlueprintAssignable, Category="InteractionQueue")
	FOnInteractionForcedDynamicSignature OnInteractionForced;

	/**
	 * Adds a new interactive actor to the interaction queue
	 * @param InteractiveActor An interactive actor to add. Must be a valid actor
	 * @return True if the interactive actor was successfully added
	 */
	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	bool AddToInteractionQueue(AActor* InteractiveActor);

	/**
	 * Removes an interactive actor to the interaction queue
	 * @param InteractiveActor An interactive actor to remove. Must be a valid actor
	 * @return True if the interactive actor was successfully removed
	 */
	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	bool RemoveFromInteractionQueue(AActor* InteractiveActor);

	/**
	 * Checks if a given actor is in the interaction queue
	 * @param Actor An interactive actor to check
	 * @return True if the actor is in the interaction queue
	 */
	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	bool IsInInteractionQueue(AActor* Actor);

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

	/**
	 * Starts interaction with the first actor in the interaction queue
	 * @return result of the interaction start
	 */
	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	EInteractionResult StartInteraction();

	/**
	 * Starts interaction with the first actor in the interaction queue
	 * @return result of the interaction finish
	 */
	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	EInteractionResult FinishInteraction();

	/**
	 * Interrupts current interaction sequence
	 * @param Interruptor An actor which interrupts the interaction
	 * @return result of the interaction interruption
	 */
	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	EInteractionResult InterruptInteraction(AActor* Interruptor);

	/**
	 * Forces interaction with the first actor in the interaction queue
	 * Usually used for immediate interactions which don't require animations
	 * @return result of the interaction
	 */
	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	EInteractionResult ForceInteraction();

	/**
	 * Registers a camera which will be used for the line of sight check
	 * @param Camera Camera component to register
	 */
	UFUNCTION(BlueprintCallable, Category="InteractionQueue")
	void RegisterCamera(UCameraComponent* Camera);

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetInteractionQueue, Category="InteractionQueue")
	TArray<AActor*> InteractionQueue;

	/**
	 * If true, the line of sight checks will be enabled if InteractionQueue isn't empty
	 */
	UPROPERTY(EditDefaultsOnly,
		BlueprintGetter=GetUseLineOfSight,
		BlueprintSetter=SetUseLineOfSight,
		Category="InteractionQueue")
	bool bUseLineOfSight = false;
	
	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;

	/**
	 * The trace channel used for line of sight checks
	 */
	UPROPERTY(EditDefaultsOnly, Category="InteractionQueue", meta=(EditCondition="bUseLineOfSight"))
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;

	/**
	 * Distance of the sphere trace which is used for line of sight checks
	 */
	UPROPERTY(EditDefaultsOnly, Category="InteractionQueue", meta=(ClampMin=1, UIMin=1, EditCondition="bUseLineOfSight"))
	float LineOfSightDistance = 500.f;

	/**
	 * Radius of the sphere trace which is used for line of sight checks
	 */
	UPROPERTY(EditDefaultsOnly, Category="InteractionQueue", meta=(ClampMin=1, UIMin=1, EditCondition="bUseLineOfSight"))
	float LineOfSightRadius = 32.f;

	/**
	 * Defines the type of debug to use for line of sigh check
	 */
	UPROPERTY(EditDefaultsOnly, Category="InteractionQueue", AdvancedDisplay, meta=(EditCondition="bUseLineOfSight"))
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::Type::None;

	/**
	 * Color of the debug trace
	 */
	UPROPERTY(EditDefaultsOnly, Category="InteractionQueue", AdvancedDisplay, meta=(EditCondition="bUseLineOfSight"))
	FLinearColor TraceColor = FColor::Red;

	/**
	 * Color of the debug tarce when the hit was registered
	 */
	UPROPERTY(EditDefaultsOnly, Category="InteractionQueue", AdvancedDisplay, meta=(EditCondition="bUseLineOfSight"))
	FLinearColor TraceHitColor = FColor::Green;

	/**
	 * How long the debug trace will be drawn
	 */
	UPROPERTY(EditDefaultsOnly, Category="InteractionQueue", AdvancedDisplay, meta=(EditCondition="bUseLineOfSight"))
	float DrawTime = 0.05f;

	UPROPERTY(Transient)
	AActor* ActorInSight = nullptr;
	
	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

	void SortInteractionQueue();

	void ToggleComponentTick();

	void CheckLineOfSight(const float DeltaTime, FHitResult& OutHitResult) const;

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	void PrintLog(const FString& Message);
	
	void PrintWarning(const FString& Message);
	
	void PrintError(const FString& Message);

	void GetNames(FString& OutOwnerName, const AActor* Actor, FString& OutActorName) const;

	static void GetInteractionResultName(EInteractionResult Result, FString& OutResultName);
#endif
};
