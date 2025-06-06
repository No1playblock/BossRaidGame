// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AN/AN_ShootCheck.h"
#include "Character/GASCharacterPlayer.h"
#include "AbilitySystemBlueprintLibrary.h"

UAN_ShootCheck::UAN_ShootCheck()
{
}

FString UAN_ShootCheck::GetNotifyName_Implementation() const
{
	return TEXT("AN_ShootCheck");
}

void UAN_ShootCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			FGameplayEventData PayloadData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerTag, PayloadData);

		}
	}
}
