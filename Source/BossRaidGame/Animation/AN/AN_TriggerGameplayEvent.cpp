// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AN/AN_TriggerGameplayEvent.h"
#include "Character/GASCharacterPlayer.h"
#include "AbilitySystemBlueprintLibrary.h"

UAN_TriggerGameplayEvent::UAN_TriggerGameplayEvent()
{
}

FString UAN_TriggerGameplayEvent::GetNotifyName_Implementation() const
{
	return TEXT("AN_TriggerGameplayEvent");
}

void UAN_TriggerGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			if (!TriggerTag.IsValid()) return;


			FGameplayEventData Payload;
			Payload.EventTag = TriggerTag;
			Payload.Instigator = OwnerActor;
			Payload.Target = OwnerActor;

			FGameplayEventData PayloadData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerTag, PayloadData);

		}
	}
}
