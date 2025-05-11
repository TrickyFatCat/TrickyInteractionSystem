// MIT License Copyright (c) Artyom "Tricky Fat Cat" Volkov


#include "InteractionQueueComponent.h"

#include "TrickyInteractionInterface.h"
#include "TrickyInteractionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogInteractionQueueComponent);

UInteractionQueueComponent::UInteractionQueueComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UInteractionQueueComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetComponentTickEnabled(false);
	ActorsToIgnore.AddUnique(GetOwner());
}

void UInteractionQueueComponent::TickComponent(float DeltaTime,
                                               ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult HitResult;
	CheckLineOfSight(DeltaTime, HitResult);

	if (HitResult.bBlockingHit)
	{
		const FVector TraceDirection = UKismetMathLibrary::GetDirectionUnitVector(
			GetOwner()->GetActorLocation(), HitResult.TraceEnd);
		const FVector ImpactDirection = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(),
			HitResult.Location);
		const float DotProduct = FVector::DotProduct(TraceDirection, ImpactDirection);
		ActorInSight = DotProduct < 0.f ? nullptr : HitResult.GetActor();

		if (UTrickyInteractionLibrary::IsActorInteractive(ActorInSight) && IsInInteractionQueue(ActorInSight))
		{
			FInteractionData InteractionData;
			UTrickyInteractionLibrary::GetActorInteractionData(ActorInSight, InteractionData);

			if (InteractionData.bRequiresLineOfSight)
			{
				const int32 Index = InteractionQueue.IndexOfByKey(ActorInSight);
				InteractionQueue.Swap(Index, 0);
			}
			else
			{
				SortInteractionQueue();
			}
		}
		else
		{
			SortInteractionQueue();
		}
	}
}

bool UInteractionQueueComponent::AddToInteractionQueue(AActor* InteractiveActor)
{
	if (!UTrickyInteractionLibrary::IsActorInteractive(InteractiveActor) || IsInInteractionQueue(InteractiveActor))
	{
		return false;
	}

	InteractionQueue.Emplace(InteractiveActor);
	SortInteractionQueue();

	if (bUseLineOfSight && !IsComponentTickEnabled())
	{
		ToggleComponentTick();
	}

	OnActorAddedToInteractionQueue.Broadcast(this, InteractiveActor);

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	FString ActorName, OwnerName = GetOwner()->GetActorNameOrLabel();
	GetNames(OwnerName, InteractiveActor, ActorName);
	const FString Message = FString::Printf(TEXT("%s added to InteractionQueue of %s"), *ActorName, *OwnerName);
	PrintLog(Message);
#endif

	return true;
}

bool UInteractionQueueComponent::RemoveFromInteractionQueue(AActor* InteractiveActor)
{
	const bool bIsInteractiveActor = UTrickyInteractionLibrary::IsActorInteractive(InteractiveActor);

	if (!bIsInteractiveActor || !IsInInteractionQueue(InteractiveActor) || IsInteractionQueueEmpty())
	{
		return false;
	}

	if (InteractionQueue.RemoveSingle(InteractiveActor) <= 0)
	{
		return false;
	}

	SortInteractionQueue();
	OnActorRemovedFromInteractionQueue.Broadcast(this, InteractiveActor);

	if (IsInteractionQueueEmpty())
	{
		SetComponentTickEnabled(false);
	}

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	FString ActorName, OwnerName = GetOwner()->GetActorNameOrLabel();
	GetNames(OwnerName, InteractiveActor, ActorName);
	const FString Message = FString::Printf(TEXT("%s removed from InteractionQueue of %s"), *ActorName, *OwnerName);
	PrintLog(Message);
#endif

	return true;
}

bool UInteractionQueueComponent::IsInInteractionQueue(AActor* Actor)
{
	if (!UTrickyInteractionLibrary::IsActorInteractive(Actor))
	{
		return false;
	}

	return InteractionQueue.Contains(Actor);
}

void UInteractionQueueComponent::SetUseLineOfSight(bool Value)
{
	if (bUseLineOfSight == Value)
	{
		return;
	}

	bUseLineOfSight = Value;
}

EInteractionResult UInteractionQueueComponent::StartInteraction()
{
	AActor* Interactor = GetOwner();

	if (IsInteractionQueueEmpty() || !IsValid(Interactor))
	{
		return EInteractionResult::Invalid;
	}

	AActor* InteractiveActor = InteractionQueue[0];

	if (!IsValid(InteractiveActor))
	{
		return EInteractionResult::Invalid;
	}

	FInteractionData InteractionData;
	UTrickyInteractionLibrary::GetActorInteractionData(InteractiveActor, InteractionData);

	if (InteractionData.bRequiresLineOfSight && InteractiveActor != ActorInSight)
	{
		return EInteractionResult::Invalid;
	}

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	FString ActorName, OwnerName = GetOwner()->GetActorNameOrLabel();
	GetNames(OwnerName, InteractiveActor, ActorName);
#endif

	const EInteractionResult InteractionResult = ITrickyInteractionInterface::Execute_StartInteraction(InteractiveActor, Interactor);
	UTrickyInteractionLibrary::GetActorInteractionData(InteractiveActor, InteractionData);
	OnInteractionStarted.Broadcast(this, InteractiveActor, InteractionResult);

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	FString Result = "NONE";
	GetInteractionResultName(InteractionResult, Result);
	const FString Message = FString::Printf(
		TEXT("%s started interaction with %s. Result %s"), *OwnerName, *ActorName, *Result);
	PrintLog(Message);
#endif

	return InteractionResult;
}

EInteractionResult UInteractionQueueComponent::FinishInteraction()
{
	AActor* Interactor = GetOwner();

	if (IsInteractionQueueEmpty() || !IsValid(Interactor))
	{
		return EInteractionResult::Invalid;
	}

	AActor* InteractiveActor = InteractionQueue[0];

	if (!IsValid(InteractiveActor))
	{
		return EInteractionResult::Invalid;
	}

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	FString ActorName, OwnerName = GetOwner()->GetActorNameOrLabel();
	GetNames(OwnerName, InteractiveActor, ActorName);
#endif
	
	const EInteractionResult InteractionResult = ITrickyInteractionInterface::Execute_FinishInteraction(InteractiveActor, Interactor);
	OnInteractionFinished.Broadcast(this, InteractiveActor, InteractionResult);

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	FString Result = "NONE";
	GetInteractionResultName(InteractionResult, Result);
	const FString Message = FString::Printf(
		TEXT("%s finished interaction with %s. Result %s"), *OwnerName, *ActorName, *Result);
	PrintLog(Message);
#endif

	return InteractionResult;
}

EInteractionResult UInteractionQueueComponent::InterruptInteraction(AActor* Interruptor)
{
	AActor* Interactor = GetOwner();

	if (IsInteractionQueueEmpty() || !IsValid(Interactor))
	{
		return EInteractionResult::Invalid;
	}

	AActor* InteractiveActor = InteractionQueue[0];

	if (!IsValid(InteractiveActor))
	{
		return EInteractionResult::Invalid;
	}

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	FString ActorName, OwnerName = GetOwner()->GetActorNameOrLabel();
	GetNames(OwnerName, InteractiveActor, ActorName);
#endif
	
	const EInteractionResult InteractionResult = ITrickyInteractionInterface::Execute_InterruptInteraction(
		InteractiveActor, Interruptor, Interactor);
	OnInteractionInterrupted.Broadcast(this, InteractiveActor, Interruptor, InteractionResult);

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	const FString InterruptorName = Interruptor->GetActorNameOrLabel();
	FString Result = "NONE";
	GetInteractionResultName(InteractionResult, Result);
	const FString Message = FString::Printf(
		TEXT("%s interrupts %s interaction with %s. Result: %s"), *InterruptorName, *OwnerName, *ActorName, *Result);
	PrintLog(Message);
#endif

	return InteractionResult;
}

EInteractionResult UInteractionQueueComponent::ForceInteraction()
{
	AActor* Interactor = GetOwner();

	if (IsInteractionQueueEmpty() || !IsValid(Interactor))
	{
		return EInteractionResult::Invalid;
	}

	AActor* InteractiveActor = InteractionQueue[0];

	if (!IsValid(InteractiveActor))
	{
		return EInteractionResult::Invalid;
	}

	FInteractionData InteractionData;
	UTrickyInteractionLibrary::GetActorInteractionData(InteractiveActor, InteractionData);

	if (InteractionData.bRequiresLineOfSight && InteractiveActor != ActorInSight)
	{
		return EInteractionResult::Invalid;
	}

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	FString ActorName, OwnerName = GetOwner()->GetActorNameOrLabel();
	GetNames(OwnerName, InteractiveActor, ActorName);
#endif
	
	const EInteractionResult InteractionResult = ITrickyInteractionInterface::Execute_ForceInteraction(InteractiveActor, Interactor);
	OnInteractionForced.Broadcast(this, InteractiveActor, InteractionResult);

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	FString Result = "NONE";
	GetInteractionResultName(InteractionResult, Result);
	const FString Message = FString::Printf(
		TEXT("%s forced interaction with %s. Result %s"), *OwnerName, *ActorName, *Result);
	PrintLog(Message);
#endif

	return InteractionResult;
}

void UInteractionQueueComponent::RegisterCamera(UCameraComponent* Camera)
{
	if (!IsValid(Camera))
	{
		return;
	}

	if (IsValid(CameraComponent))
	{
		ActorsToIgnore.Remove(CameraComponent->GetOwner());
	}

	CameraComponent = Camera;
	ActorsToIgnore.AddUnique(CameraComponent->GetOwner());
}

void UInteractionQueueComponent::SortInteractionQueue()
{
	if (InteractionQueue.Num() <= 1)
	{
		return;
	}

	auto Predicate = [](AActor* ActorA, AActor* ActorB) -> bool
	{
		FInteractionData InteractionDataA;
		UTrickyInteractionLibrary::GetActorInteractionData(ActorA, InteractionDataA);
		const int32 WeightA = InteractionDataA.bRequiresLineOfSight ? -1 : InteractionDataA.InteractionWeight;

		FInteractionData InteractionDataB;
		UTrickyInteractionLibrary::GetActorInteractionData(ActorB, InteractionDataB);
		const int32 WeightB = InteractionDataB.bRequiresLineOfSight ? -1 : InteractionDataB.InteractionWeight;

		return WeightA >= WeightB;
	};

	Algo::Sort(InteractionQueue, Predicate);
}

void UInteractionQueueComponent::ToggleComponentTick()
{
	if (!IsValid(CameraComponent))
	{
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		const FString OwnerName = GetOwner()->GetActorNameOrLabel();
		const FString Instruction = FString::Printf(
			TEXT("Please register valid CameraComponent in InteractionQueueComponent of %s"), *OwnerName);
		const FString Message = FString::Printf(
			TEXT("Can't toggle InteractionQueueComponent tick in %s. CameraComponent is invalid.\n%s"),
			*OwnerName,
			*Instruction);
#endif
		return;
	}

	SetComponentTickEnabled(bUseLineOfSight && !IsInteractionQueueEmpty() && !IsComponentTickEnabled());
}

void UInteractionQueueComponent::CheckLineOfSight(const float DeltaTime, FHitResult& OutHitResult) const
{
	if (!IsValid(CameraComponent))
	{
		return;
	}

	FMinimalViewInfo ViewInfo;
	CameraComponent->GetCameraView(DeltaTime, ViewInfo);

	const FVector StartPoint = ViewInfo.Location;
	const FVector EndPoint = ViewInfo.Location + ViewInfo.Rotation.Vector() * LineOfSightDistance;

	UKismetSystemLibrary::SphereTraceSingle(GetOwner(),
	                                        StartPoint,
	                                        EndPoint,
	                                        LineOfSightRadius,
	                                        TraceChannel,
	                                        false,
	                                        ActorsToIgnore,
	                                        DrawDebugType,
	                                        OutHitResult,
	                                        true,
	                                        TraceColor,
	                                        TraceHitColor,
	                                        DrawTime);
}

#if WITH_EDITOR && !UE_BUILD_SHIPPING
void UInteractionQueueComponent::PrintLog(const FString& Message)
{
	UE_LOG(LogInteractionQueueComponent, Log, TEXT("%s"), *Message);
}

void UInteractionQueueComponent::PrintWarning(const FString& Message)
{
	UE_LOG(LogInteractionQueueComponent, Warning, TEXT("%s"), *Message);
}

void UInteractionQueueComponent::PrintError(const FString& Message)
{
	UE_LOG(LogInteractionQueueComponent, Error, TEXT("%s"), *Message);
}

void UInteractionQueueComponent::GetNames(FString& OutOwnerName, const AActor* Actor, FString& OutActorName) const
{
	OutOwnerName = GetOwner()->GetActorNameOrLabel();
	OutActorName = IsValid(Actor) ? Actor->GetActorNameOrLabel() : TEXT("NULL");
}

void UInteractionQueueComponent::GetInteractionResultName(EInteractionResult Result, FString& OutResultName)
{
	OutResultName = StaticEnum<EInteractionResult>()->GetNameStringByValue(static_cast<int64>(Result));
}
#endif
