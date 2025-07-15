// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Interact.h"
#include "Character/GASCharacterPlayer.h"
#include "Interface/InteractionInterface.h"
UGA_Interact::UGA_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// �����Ƽ�� ������ ĳ���͸� ������
	AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(ActorInfo->AvatarActor.Get());
	if (!PlayerCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true); // ĳ���� ������ �����Ƽ ����
		return;
	}

	// ĳ���Ͱ� ���� ��ȣ�ۿ� ������ ���͸� ������ �ִ��� Ȯ��
	AActor* InteractableActor = PlayerCharacter->GetCurrentInteractableActor();
	if (InteractableActor && InteractableActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		// �������̽��� Interact �Լ� ȣ��
		UE_LOG(LogTemp, Warning, TEXT("Interact"));
		Cast<IInteractionInterface>(InteractableActor)->Interact(PlayerCharacter);
	}
	
	// �� ���� �������Ƿ� �����Ƽ ����
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
