// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Dash.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Prop/GhostTrailActor.h"
#include "Character/GASCharacterPlayer.h"
#include "Components/SkillTreeComponent.h"
UGA_Dash::UGA_Dash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 이 태그를 통해 잔상 이펙트(GameplayCue)가 켜짐 (BP에서 설정 필요)
	// AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayCue.Skill.Dash")));
}

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Log, TEXT("UGA_Dash::ActivateAbility called"));
	// 쿨타임 및 자원 확인
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		GhostTimerHandle,
		this,
		&UGA_Dash::SpawnGhostTrail,
		GhostSpawnInterval,
		true // true = 반복 실행
	);
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
	}
	SpawnGhostTrail();

	FVector InputVector = Character->GetCharacterMovement()->GetLastInputVector();
	FVector DashDir = InputVector.IsNearlyZero() ? Character->GetActorForwardVector() : InputVector.GetSafeNormal();

	DashDir.Z = 0.0f;
	DashDir = DashDir.GetSafeNormal();
	UAbilityTask_ApplyRootMotionConstantForce* Task = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		FName("DashTask"),
		DashDir,
		DashStrength,
		DashDuration,
		false,
		nullptr,
		ERootMotionFinishVelocityMode::SetVelocity,
		FVector::ZeroVector,
		0.0f,
		false
	);

	if (Task)
	{
		Task->OnFinish.AddDynamic(this, &UGA_Dash::OnDashFinished);
		Task->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}
void UGA_Dash::SpawnGhostTrail()
{
	// 능력 종료되었거나 아바타 없으면 중단
	if (!IsActive() || !GetAvatarActorFromActorInfo()) return;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !GhostActorClass) return;

	FVector SpawnLoc = Character->GetMesh()->GetComponentLocation();
	FRotator SpawnRot = Character->GetMesh()->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 유령 액터 스폰하고 포즈 복사 명령 내림
	AGhostTrailActor* Ghost = GetWorld()->SpawnActor<AGhostTrailActor>(GhostActorClass, SpawnLoc, SpawnRot, SpawnParams);
	if (Ghost)
	{
		Ghost->InitGhost(Character->GetMesh());
	}
}
void UGA_Dash::OnDashFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(GhostTimerHandle);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}