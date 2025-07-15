// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_SpawnActor.h"

UGA_SpawnActor::UGA_SpawnActor()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_SpawnActor::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 스폰할 액터 클래스가 지정되었는지, 이벤트 데이터가 유효한지 확인합니다.
	if (SpawnActorClass && TriggerEventData)
	{
		// 이벤트 데이터로부터 석상 액터를 가져옵니다.
		const AActor* StatueActor = TriggerEventData->Target;
		if (StatueActor)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				// 석상의 위치에 액터를 스폰합니다.
				FVector SpawnLocation = StatueActor->GetActorLocation();
				SpawnLocation.Z -= 125.0f; // 석상 위에 스폰하도록 Z축 위치를 조정
				FRotator SpawnRotation = StatueActor->GetActorRotation();

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwningActorFromActorInfo();
				SpawnParams.Instigator = GetOwningActorFromActorInfo()->GetInstigator();

				World->SpawnActor<AActor>(SpawnActorClass, SpawnLocation, SpawnRotation, SpawnParams);
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
