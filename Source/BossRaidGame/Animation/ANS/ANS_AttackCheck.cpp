// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ANS/ANS_AttackCheck.h"
#include "Character/GASCharacterPlayer.h"
#include "AbilitySystemBlueprintLibrary.h"

UANS_AttackCheck::UANS_AttackCheck()
{
}

FString UANS_AttackCheck::GetNotifyName_Implementation() const
{


	return TEXT("ANS_AttackCheck");
}

void UANS_AttackCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			FGameplayEventData PayloadData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerStartTag, PayloadData);

			UE_LOG(LogTemp, Warning, TEXT("NotifyBegin"));
		}
	}
	

	
}


void UANS_AttackCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			FGameplayEventData PayloadData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerEndTag, PayloadData);

			UE_LOG(LogTemp, Warning, TEXT("NotifyEnd"));
		}
	}
}
