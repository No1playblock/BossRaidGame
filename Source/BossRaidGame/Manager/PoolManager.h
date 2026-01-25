// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolManager.generated.h"

USTRUCT(BlueprintType)
struct FActorPoolConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 10;
};
USTRUCT(BlueprintType)
struct FEffectPoolConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParticleSystem> EffectTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 10;
};

UCLASS()
class BOSSRAIDGAME_API APoolManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APoolManager();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	// 디테일 패널에서 설정할 리스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TArray<FActorPoolConfig> ActorPools;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TArray<FEffectPoolConfig> EffectPools;

};
