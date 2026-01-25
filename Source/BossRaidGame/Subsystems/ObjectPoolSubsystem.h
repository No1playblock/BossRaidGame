// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectPoolSubsystem.generated.h"

/**
 * 
 */
USTRUCT()
struct FActorPoolArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<AActor>> Items;
};

UCLASS()
class BOSSRAIDGAME_API UObjectPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	/**
	 * 풀에서 액터를 가져옵니다. (없으면 새로 생성)
	 * @param ActorClass 스폰할 액터 클래스
	 * @param Location 스폰 위치
	 * @param Rotation 스폰 회전
	 * @return 스폰된 액터 포인터
	 */
	UFUNCTION(BlueprintCallable, Category = "ObjectPool", meta = (DeterminesOutputType = "ActorClass"))
	AActor* SpawnFromPool(TSubclassOf<AActor> ActorClass, FVector Location, FRotator Rotation, AActor* Owner = nullptr, APawn* Instigator = nullptr);
	/**
	 * 액터를 풀로 반환합니다. (Destroy 대신 사용)
	 * @param ActorToReturn 반환할 액터
	 */
	UFUNCTION(BlueprintCallable, Category = "ObjectPool")
	void ReturnToPool(AActor* ActorToReturn);


	UFUNCTION(BlueprintCallable, Category = "ObjectPool")
	void WarmUpPool(TSubclassOf<AActor> ActorClass, int32 Count);
private:
	/**
	 * 풀링된 액터들을 저장하는 보관함
	 * Key: 액터 클래스 (UClass*)
	 * Value: 해당 클래스의 비활성화된 액터 목록 (Struct)
	 */
	UPROPERTY()
	TMap<UClass*, FActorPoolArray> ObjectPool;
};
