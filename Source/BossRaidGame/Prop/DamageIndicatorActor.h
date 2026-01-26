// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageIndicatorActor.generated.h"

UCLASS()
class BOSSRAIDGAME_API ADamageIndicatorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageIndicatorActor();
	
	UFUNCTION(BlueprintCallable)
	void InitializeDamage(float DamageAmount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UWidgetComponent> DamageWidgetComp;

};
