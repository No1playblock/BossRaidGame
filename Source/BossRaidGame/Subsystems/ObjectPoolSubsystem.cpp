// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ObjectPoolSubsystem.h"
#include "Interface/PoolableInterface.h"
#include "GameFramework/Actor.h"

void UObjectPoolSubsystem::WarmUpPool(TSubclassOf<AActor> ActorClass, int32 Count)
{
	if (!ActorClass || !GetWorld()) return;

	for (int32 i = 0; i < Count; i++)
	{
		// 생성
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* NewActor = GetWorld()->SpawnActor<AActor>(ActorClass, FVector(0, 0, -5000), FRotator::ZeroRotator, SpawnParams);

		if (NewActor)
		{
			//반환
			ReturnToPool(NewActor);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Object Pool Warmed Up: %s (%d created)"), *ActorClass->GetName(), Count);
}


AActor* UObjectPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> ActorClass, FVector Location, FRotator Rotation, AActor* Owner, APawn* Instigator)
{
	if (!ActorClass || !GetWorld())
	{
		return nullptr;
	}

	AActor* SpawnedActor = nullptr;

	// 풀(Map)에 해당 클래스 키가 있고, 보관된 액터가 있는지 확인
	if (ObjectPool.Contains(ActorClass) && ObjectPool[ActorClass].Items.Num() > 0)
	{
		//꺼내고
		SpawnedActor = ObjectPool[ActorClass].Items.Pop();

		if (IsValid(SpawnedActor))
		{
			// 위치 및 회전 설정 (TeleportFlag로 물리 충돌 방지)
			SpawnedActor->SetActorLocationAndRotation(Location, Rotation, false, nullptr, ETeleportType::ResetPhysics);

			//Owner 및 Instigator 설정
			if (Owner) SpawnedActor->SetOwner(Owner);
			if (Instigator) SpawnedActor->SetInstigator(Instigator);

			//활성화
			SpawnedActor->SetActorHiddenInGame(false);
			SpawnedActor->SetActorEnableCollision(true);
			SpawnedActor->SetActorTickEnabled(true);

			// 인터페이스 호출 (초기화)
			if (SpawnedActor && SpawnedActor->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
			{
				IPoolableInterface* Poolable = Cast<IPoolableInterface>(SpawnedActor);
				if (Poolable)
				{
					Poolable->OnPoolSpawned();
				}
			}

			return SpawnedActor;
		}
	}

	// 풀에 없으면 새로 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Instigator;

	SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClass, Location, Rotation, SpawnParams);
	

	// 새로 생성된 액터도 인터페이스 호출(일관성 유지)
	if (SpawnedActor && SpawnedActor->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
	{
		IPoolableInterface* Poolable = Cast<IPoolableInterface>(SpawnedActor);
		if (Poolable)
		{
			Poolable->OnPoolSpawned();
		}
	}

	return SpawnedActor;
}

void UObjectPoolSubsystem::ReturnToPool(AActor* ActorToReturn)
{
	if (!IsValid(ActorToReturn) || !ActorToReturn->IsValidLowLevel())
	{
		return;
	}

	// 인터페이스 호출 (정리 로직: 타이머 정지, 이펙트 끄기 등)
	if (ActorToReturn->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
	{
		IPoolableInterface* Poolable = Cast<IPoolableInterface>(ActorToReturn);
		if (Poolable)
		{
			Poolable->OnPoolDespawned();
		}
	}

	//비활성화 처리
	ActorToReturn->SetActorHiddenInGame(true);
	ActorToReturn->SetActorEnableCollision(false);
	ActorToReturn->SetActorTickEnabled(false);

	//
	// ActorToReturn->SetActorLocation(FVector(0.f, 0.f, -10000.f));

	// 풀에 저장 (해당 클래스 타입의 배열을 찾아 추가)
	UClass* ActorClass = ActorToReturn->GetClass();
	ObjectPool.FindOrAdd(ActorClass).Items.Add(ActorToReturn);
}