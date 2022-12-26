// MIT License Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov

#pragma once

#include "CoreMinimal.h"
#include "InteractionLibrary.h"
#include "Components/BoxComponent.h"
#include "BoxInteractionComponent.generated.h"

/**
 * A simple box interaction component which adds the owner to the interaction queue. 
 */
UCLASS(ClassGroup=(TrickyInteraction), meta=(BlueprintSpawnableComponent))
class TRICKYINTERACTIONSYSTEM_API UBoxInteractionComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UBoxInteractionComponent();

	UFUNCTION(BlueprintSetter, Category="TrickyInteractionSystem")
	void SetInteractionData(const FInteractionData& Value);

	UFUNCTION(BlueprintCallable, Category="TrickyInteractionSystem")
	void SetInteractionMessage(AActor* Actor, const FString& Message);

	UFUNCTION(BlueprintGetter, Category="TrickyInteractionSystem")
	FInteractionData GetInteractionData() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly,
		BlueprintGetter=GetInteractionData,
		BlueprintSetter=SetInteractionData,
		Category="Interaction",
		meta=(AllowPrivateAccess))
	FInteractionData InteractionData;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                            AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp,
	                            int32 OtherBodyIndex,
	                            bool bFromSweep,
	                            const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent,
	                          AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp,
	                          int32 OtherBodyIndex);
};
