// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AN_TriggerGameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UAN_TriggerGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
	
	UAN_TriggerGameplayEvent();

	virtual FString GetNotifyName_Implementation() const override;

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag TriggerTag;
};
