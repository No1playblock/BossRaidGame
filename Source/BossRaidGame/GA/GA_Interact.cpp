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
	
	// 어빌리티를 실행한 캐릭터를 가져옴
	AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(ActorInfo->AvatarActor.Get());
	if (!PlayerCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true); // 캐릭터 없으면 어빌리티 종료
		return;
	}

	// 캐릭터가 현재 상호작용 가능한 액터를 가지고 있는지 확인
	AActor* InteractableActor = PlayerCharacter->GetCurrentInteractableActor();
	if (InteractableActor && InteractableActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		// 인터페이스의 Interact 함수 호출
		UE_LOG(LogTemp, Warning, TEXT("Interact"));
		Cast<IInteractionInterface>(InteractableActor)->Interact(PlayerCharacter);
	}
	
	// 할 일을 마쳤으므로 어빌리티 종료
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
