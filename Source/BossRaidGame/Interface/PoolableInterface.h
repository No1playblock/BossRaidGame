// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPoolableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BOSSRAIDGAME_API IPoolableInterface
{
	GENERATED_BODY()


public:
/**
	 * [풀에서 꺼내질 때 호출]
	 * - 초기화 로직 (속도 리셋, 이펙트 켜기, 체력 복구 등)을 구현해야함
	 */
	UFUNCTION(Category = "Object Pool")
	virtual void OnPoolSpawned() =0 ;

	/**
	 * [풀로 돌아갈 때 호출]
	 * - 정리 로직 (타이머 정지, 이펙트 끄기, 상태 초기화 등)을 구현해야함
	 */
	UFUNCTION(Category = "Object Pool")
	virtual void OnPoolDespawned() =0 ;
};
