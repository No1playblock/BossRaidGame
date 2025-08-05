// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_MultiHitAtLocation.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UAT_MultiHitAtLocation* UAT_MultiHitAtLocation::MultiHitAtLocation(UGameplayAbility* OwningAbility, FName TargetLocationKeyName, int32 InNumberOfHits, float InDelayBetweenHits, float InAreaRadius, TEnumAsByte<ECollisionChannel> InTargetChannel, UParticleSystem* InAreaVFX, USoundBase* InAreaSFX)
{
	UAT_MultiHitAtLocation* MyTask = NewAbilityTask<UAT_MultiHitAtLocation>(OwningAbility);
	MyTask->TargetLocationKeyName = TargetLocationKeyName;
	MyTask->NumberOfHits = InNumberOfHits;
	MyTask->DelayBetweenHits = InDelayBetweenHits;
	MyTask->AreaRadius = InAreaRadius;
	MyTask->TargetChannel = InTargetChannel;
	MyTask->AreaVFX = InAreaVFX;
	MyTask->AreaSFX = InAreaSFX;
	return MyTask;
}

void UAT_MultiHitAtLocation::Activate()
{
	Super::Activate();
	
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActor());
	if (AvatarPawn)
	{
		AController* Controller = AvatarPawn->GetController();

		if (AAIController* AIController = Cast<AAIController>(Controller))
		{
			BlackboardComp = AIController->GetBlackboardComponent();
		}
	}

	CurrentHitCount = 0;
	PerformNextHit();
}


void UAT_MultiHitAtLocation::PerformNextHit()
{
	if (CurrentHitCount >= NumberOfHits)
	{
		if (OnFinished.IsBound())
		{
			OnFinished.Broadcast();
		}
		EndTask();
		return;
	}

	if (!BlackboardComp.IsValid())
	{
		EndTask();
		return;
	}

	const FVector TargetLocation = BlackboardComp->GetValueAsVector(TargetLocationKeyName);
	if (TargetLocation == FVector::ZeroVector)
	{
		EndTask();
		return;
	}

	AActor* OwnerActor = GetAvatarActor();
	UWorld* World = GetWorld();
	if (!World || !OwnerActor)
	{
		EndTask();
		return;
	}

	if (AreaVFX)
	{
		FVector VFXLocation = TargetLocation;
		VFXLocation.Z -= 90.0f; 
		UGameplayStatics::SpawnEmitterAtLocation(World, AreaVFX, VFXLocation);
	}
	if (AreaSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(World, AreaSFX, TargetLocation);
	}

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(AreaRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerActor);

	World->OverlapMultiByChannel(OverlapResults, TargetLocation, FQuat::Identity, TargetChannel, SphereShape, QueryParams);

	const FColor DebugColor = OverlapResults.Num() > 0 ? FColor::Green : FColor::Red;
	float DebugLifetime = DelayBetweenHits > 0.f ? DelayBetweenHits : 2.0f;
	DrawDebugSphere(World, TargetLocation, AreaRadius, 24, DebugColor, false, DebugLifetime);

	if (OverlapResults.Num() > 0 && OnHit.IsBound())
	{
		OnHit.Broadcast(OverlapResults);
	}

	CurrentHitCount++;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAT_MultiHitAtLocation::PerformNextHit, DelayBetweenHits, false);
}

void UAT_MultiHitAtLocation::OnDestroy(bool bInOwnerFinished)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	Super::OnDestroy(bInOwnerFinished);
}
