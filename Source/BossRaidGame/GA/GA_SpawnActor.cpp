// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_SpawnActor.h"

UGA_SpawnActor::UGA_SpawnActor()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_SpawnActor::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// ������ ���� Ŭ������ �����Ǿ�����, �̺�Ʈ �����Ͱ� ��ȿ���� Ȯ���մϴ�.
	if (SpawnActorClass && TriggerEventData)
	{
		// �̺�Ʈ �����ͷκ��� ���� ���͸� �����ɴϴ�.
		const AActor* StatueActor = TriggerEventData->Target;
		if (StatueActor)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				// ������ ��ġ�� ���͸� �����մϴ�.
				FVector SpawnLocation = StatueActor->GetActorLocation();
				SpawnLocation.Z -= 125.0f; // ���� ���� �����ϵ��� Z�� ��ġ�� ����
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
