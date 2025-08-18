// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_AutoRaser.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkillTreeComponent.h"
#include "GameData/SkillTreeData.h"
#include "AbilitySystemComponent.h"
#include "TimerManager.h"
#include "Prop/RaserOrb.h"

UGA_AutoRaser::UGA_AutoRaser()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationBlockedTags.AddTag(CooldownTag);

}

void UGA_AutoRaser::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!OwnerCharacter) return;

	UE_LOG(LogTemp, Warning, TEXT("AutoRaser"));

	// 기본 데미지 값
	float Damage = 100.f;
	float CooldownTime = 1.0f; // 기본 쿨타임

	// SkillTreeData로부터 Damage 값 추출
	if (USkillTreeComponent* SkillComp = OwnerCharacter->FindComponentByClass<USkillTreeComponent>())
	{
		const FSkillTreeDataRow* SkillData = SkillComp->FindSkillDataByGrantedAbility(GetClass());
		if (SkillData)
		{
			Damage = SkillData->SkillDamage;
			CooldownTime = SkillData->SkillCoolTime;
			//UE_LOG(LogTemp, Warning, TEXT("AutoRaser Damage: %f, CooldownTime: %f"), Damage, CooldownTime);
		}
	}
	if (CooldownEffectClass)
	{
		// 쿨다운 이펙트의 지속시간을 SkillData에서 가져온 값으로 설정하여 적용
		FGameplayEffectSpecHandle CooldownSpecHandle = MakeOutgoingGameplayEffectSpec(CooldownEffectClass);
		CooldownSpecHandle.Data->SetDuration(CooldownTime, true);
		CooldownSpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CooldownSpecHandle);
		UE_LOG(LogTemp, Warning, TEXT("Applied Cooldown Effect: %s with Duration: %f"), *CooldownEffectClass->GetName(), CooldownTime);
	}
	SpawnOrbsAndFire(OwnerCharacter, Damage);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_AutoRaser::SpawnOrbsAndFire(ACharacter* OwnerCharacter, float Damage)
{
	if (!OrbActorClass || !OwnerCharacter) return;

	UWorld* World = OwnerCharacter->GetWorld();
	if (!World) return;

	FVector Origin = OwnerCharacter->GetActorLocation();
	Origin.Z += 50.0f;
	const FVector Left = OwnerCharacter->GetActorRightVector()*-1.f;   // 중심 기준 축 (Y축)
	const FVector Up = OwnerCharacter->GetActorUpVector();         // 회전 축 (Z축)

	
	const float Radius = OrbSpawnRadius;
	const float AngleStep = 180.f / (OrbCount - 1); // 반원 각도 간격

	for (int32 i = 0; i < OrbCount; ++i)
	{
		const float AngleDeg = -90.f + (AngleStep * i); // -90 ~ +90
		const float AngleRad = FMath::DegreesToRadians(AngleDeg);

		// X축 기준 반원 → YZ 평면에 반원
		FVector Offset =
			FMath::Cos(AngleRad) * Up +   // 중심 축 (Y축)
			FMath::Sin(AngleRad) * Left;       // 위아래 방향

		FVector SpawnLocation = Origin + Offset * Radius;
		FRotator SpawnRotation = Offset.Rotation();

		ARaserOrb* Orb = World->SpawnActor<ARaserOrb>(OrbActorClass, SpawnLocation, SpawnRotation);
		if (Orb)
		{
			Orb->Initialize(OwnerCharacter, Damage, DamageEffectClass, TargetChannel);

			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, [Orb]()
				{
					if (IsValid(Orb))
					{
						Orb->FireLaser();
					}
				}, FireDelay, false);
		}
	}
}