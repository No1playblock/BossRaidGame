// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/PoolManager.h"
#include "SubSystems/ObjectPoolSubsystem.h"
#include "SubSystems/EffectPoolSubsystem.h"

// Sets default values
APoolManager::APoolManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APoolManager::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	if (!World) return;

	// 액터 풀링
	if (UObjectPoolSubsystem* ObjSys = World->GetSubsystem<UObjectPoolSubsystem>())
	{
		for (const FActorPoolConfig& Config : ActorPools)
		{
			if (Config.ActorClass && Config.Count > 0)
			{
				ObjSys->WarmUpPool(Config.ActorClass, Config.Count);
				UE_LOG(LogTemp, Warning, TEXT("PoolManager: Warmed up pool for %s with count %d"), *Config.ActorClass->GetName(), Config.Count);
			}
		}
	}

	//이펙트 풀링
	if (UEffectPoolSubsystem* EffSys = World->GetSubsystem<UEffectPoolSubsystem>())
	{
		for (const FEffectPoolConfig& Config : EffectPools)
		{
			if (Config.EffectTemplate && Config.Count > 0)
			{
				EffSys->WarmUpPool(Config.EffectTemplate, Config.Count);
				UE_LOG(LogTemp, Warning, TEXT("Effect: %d"), Config.Count);
			}
		}
	}
}



