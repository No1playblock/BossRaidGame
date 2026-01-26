// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_SpawnActor.h"
#include "Character/GASCharacterPlayer.h"
#include "Subsystems/ObjectPoolSubsystem.h"
#include "Components/SkillTreeComponent.h"
#include "Interface/SkillSpawnedActorInterface.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameFramework/Actor.h"
UGA_SpawnActor::UGA_SpawnActor()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_SpawnActor::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(AvatarActor);

	float SkillDamage = 0.0f;
	float SkillCooldownTime = 0.0f;

	// 스킬 트리 데이터 조회 및 쿨타임/몽타주 처리
	if (PlayerCharacter)
	{
		if (USkillTreeComponent* SkillComp = PlayerCharacter->FindComponentByClass<USkillTreeComponent>())
		{
			// 현재 어빌리티 클래스로 스킬 데이터 찾기
			const FSkillTreeDataRow* SkillData = SkillComp->FindSkillDataByGrantedAbility(GetClass());
			if (SkillData)
			{
				SkillDamage = SkillData->SkillDamage;
				SkillCooldownTime = SkillData->SkillCoolTime;
			}
		}

		//쿨타임 수동 적용 
		if (CooldownEffectClass && SkillCooldownTime > 0.0f)
		{
			FGameplayEffectSpecHandle CooldownSpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CooldownEffectClass);
			if (CooldownSpecHandle.Data.IsValid())
			{
				CooldownSpecHandle.Data->SetDuration(SkillCooldownTime, true);
				if (CooldownTag.IsValid())
				{
					CooldownSpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
				}
				ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CooldownSpecHandle);
			}
		}

		//몽타주 재생
		if (MontageToPlay)
		{
			PlayerCharacter->PlayAnimMontage(MontageToPlay);
		}
	}

	// 소환 위치 결정
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	AActor* AnchorActor = AvatarActor; // 기본 기준점은 나 자신

	if (TriggerEventData && TriggerEventData->TargetData.Num() > 0)
	{
		const FGameplayAbilityTargetData* TargetData = TriggerEventData->TargetData.Get(0);
		if (TargetData)
		{
			if (TargetData->HasHitResult())
			{
				SpawnLocation = TargetData->GetHitResult()->Location;
				// 필요 시 회전값: SpawnRotation = TargetData->GetHitResult()->ImpactNormal.Rotation();
			}
			else if (TargetData->HasEndPoint())
			{
				SpawnLocation = TargetData->GetEndPoint();
			}
		}
	}
	
	//else if (TriggerEventData && TriggerEventData->Target)
	//{
	//	AnchorActor = const_cast<AActor*>(TriggerEventData->Target);
	//}

	if (SpawnLocation.IsZero() && AnchorActor)
	{
		SpawnLocation = AnchorActor->GetActorLocation() + (AnchorActor->GetActorQuat().RotateVector(SpawnOffset));
		SpawnRotation = AnchorActor->GetActorRotation();
	}

	// 오브젝트 풀을 통한 소환
	AActor* NewActor = nullptr;

	if (SpawnActorClass)
	{
		if (UWorld* World = GetWorld())
		{
			// 서브시스템 가져오기
			if (UObjectPoolSubsystem* PoolSys = World->GetSubsystem<UObjectPoolSubsystem>())
			{
				// Owner와 Instigator 설정
				AActor* OwnerActor = ActorInfo->OwnerActor.Get();
				APawn* InstigatorPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());

				// 풀에서 소환
				NewActor = PoolSys->SpawnFromPool(SpawnActorClass, SpawnLocation, SpawnRotation, OwnerActor, InstigatorPawn);
			}
			else
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = ActorInfo->OwnerActor.Get();
				SpawnParams.Instigator = Cast<APawn>(ActorInfo->AvatarActor.Get());
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				NewActor = World->SpawnActor<AActor>(SpawnActorClass, SpawnLocation, SpawnRotation, SpawnParams);
			}
		}
	}

	// 소환된 액터 초기화
	if (NewActor)
	{
		// 인터페이스를 통해 데미지와 효과 정보 전달
		if (NewActor->GetClass()->ImplementsInterface(USkillSpawnedActorInterface::StaticClass()))
		{
			ISkillSpawnedActorInterface::Execute_InitializeInfo(NewActor, SkillDamage, DamageEffectClass);
		}
	}

	// 어빌리티 종료
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
