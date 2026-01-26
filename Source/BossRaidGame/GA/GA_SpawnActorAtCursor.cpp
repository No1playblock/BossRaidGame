// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_SpawnActorAtCursor.h"
#include "Character/GASCharacterPlayer.h" 
#include "Components/SkillTreeComponent.h"
#include "GameFramework/Character.h"
#include "Interface/SkillSpawnedActorInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Subsystems/ObjectPoolSubsystem.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
UGA_SpawnActorAtCursor::UGA_SpawnActorAtCursor()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_SpawnActorAtCursor::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	// 필수 데이터 체크
	if (!ActorInfo->PlayerController.IsValid() || !SpawnActorClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APlayerController* PC = ActorInfo->PlayerController.Get();
	if (!PC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const AActor* AnchorActor = nullptr;
	AActor* OwnerActor = nullptr;
	APawn* InstigatorActor = nullptr;

	// TriggerEventData에 타깃이 있다면 그 타깃이 기준 
	if (TriggerEventData && TriggerEventData->Target)
	{
		AnchorActor = TriggerEventData->Target;
		// 이벤트로 트리거된 경우, Owner는 ActorInfo에서
		OwnerActor = GetOwningActorFromActorInfo();
		InstigatorActor = GetOwningActorFromActorInfo()->GetInstigator();
	}
	else
	{
		AActor* AvatarActor = ActorInfo->AvatarActor.Get();
		AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(AvatarActor);

		if (PlayerCharacter)
		{
			float CooldownTime = 0.0f;
			float SkillDamage = 0.0f; // 필요시 멤버변수에 저장하여 스폰될 액터에 전달

			// 스킬 트리에서 데이터 가져오기
			if (USkillTreeComponent* SkillComp = PlayerCharacter->FindComponentByClass<USkillTreeComponent>())
			{
				const FSkillTreeDataRow* SkillData = SkillComp->FindSkillDataByGrantedAbility(GetClass());
				if (SkillData)
				{
					Damage = SkillData->SkillDamage; // 데미지 값 캐싱 필요시 사용
					CooldownTime = SkillData->SkillCoolTime;
				}
			}

			// 동적 쿨타임 이펙트 적용 (스킬 트리의 시간 적용)
			if (CooldownEffectClass && CooldownTime > 0.0f)
			{
				FGameplayEffectSpecHandle CooldownSpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CooldownEffectClass);
				if (CooldownSpecHandle.Data.IsValid())
				{
					CooldownSpecHandle.Data->SetDuration(CooldownTime, true);
					if (CooldownTag.IsValid())
					{
						CooldownSpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
					}
					ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CooldownSpecHandle);
				}
			}

			// 애니메이션 몽타주 재생
			if (MontageToPlay)
			{
				PlayerCharacter->PlayAnimMontage(MontageToPlay);
			}
		}

		// 타깃 정보가 없으므로 내 위치를 기준으로 함
		AnchorActor = AvatarActor;
		OwnerActor = AvatarActor;
		InstigatorActor = AvatarActor->GetInstigator();
	}

	// 기준이 되는 액터가 없으면 취소
	if (!AnchorActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	//*화면 정중앙 구하기*//

	FVector CamLocation;
	FRotator CamRotation;

	// 카메라의 위치와 바라보는 방향을 가져옵니다.
	PC->GetPlayerViewPoint(CamLocation, CamRotation);

	FVector TraceStart = CamLocation;
	FVector TraceEnd = CamLocation + (CamRotation.Vector() * 50000.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ActorInfo->AvatarActor.Get()); // 시전자는 무시

	// 레이저 발사
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECollisionChannel::ECC_Visibility, // 적절한 채널 선택
		Params
	);

	FVector TargetLocation = bHit ? HitResult.Location : TraceEnd;

	if (bHit)
	{
		TargetLocation.Z += 10.0f;
	}


	AActor* NewActor = nullptr;
	if (UObjectPoolSubsystem* PoolSys = GetWorld()->GetSubsystem<UObjectPoolSubsystem>())
	{
		//풀에서 스폰 (OnPoolSpawned가 내부에서 호출됨)
		NewActor = PoolSys->SpawnFromPool(SpawnActorClass, TargetLocation, FRotator(), OwnerActor, InstigatorActor);
	}
	else
	{
		// 혹시 서브시스템이 없으면 일반 스폰
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerActor;
		SpawnParams.Instigator = InstigatorActor;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		NewActor = GetWorld()->SpawnActor<AActor>(SpawnActorClass, TargetLocation, FRotator(), SpawnParams);
	}

	if (NewActor)
	{

		//혹시 모르니 중복
		NewActor->SetOwner(OwnerActor);
		NewActor->SetInstigator(InstigatorActor);

		//인터페이스 초기화 (데미지 전달)
		if (NewActor->Implements<USkillSpawnedActorInterface>())
		{
			ISkillSpawnedActorInterface::Execute_InitializeInfo(NewActor, Damage, DamageEffectClass);
		}

		SpawnedActor = NewActor;
	}


	UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag(FName("Event.Montage.Spawn")), // 태그 직접 입력 혹은 변수 사용
		nullptr,
		false,
		true // 정확히 일치하는 태그만 받음
	);

	if (WaitTask)
	{
		// 이벤트가 오면 OnGameplayEventReceived 함수 실행하라고 연결
		WaitTask->EventReceived.AddDynamic(this, &UGA_SpawnActorAtCursor::OnGameplayEventReceived);

		// 감시 시작
		WaitTask->ReadyForActivation();
	}
	else
	{
		// 태스크 생성 실패하면 그냥 종료
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

}

void UGA_SpawnActorAtCursor::OnGameplayEventReceived(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("GA: Event Received! Triggering Skill..."));

	//저장해둔 액터 확인
	if (SpawnedActor && IsValid(SpawnedActor))
	{
		// 인터페이스 확인 후 발사 명령
		if (SpawnedActor->Implements<USkillSpawnedActorInterface>())
		{
			//발동
			ISkillSpawnedActorInterface::Execute_TriggerSkill(SpawnedActor);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
