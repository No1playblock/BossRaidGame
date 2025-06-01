// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "ANS_AttackCheck.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UANS_AttackCheck : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_AttackCheck();
protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)override;

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag TriggerStartTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag TriggerEndTag;

	//TObjectPtr<class UAbilitySystemComponent> ASC;

};
