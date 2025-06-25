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
}

void UGA_AutoRaser::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!OwnerCharacter) return;

	UE_LOG(LogTemp, Warning, TEXT("AutoRaser"));

	// �⺻ ������ ��
	float Damage = 100.f;

	// SkillTreeData�κ��� Damage �� ����
	if (USkillTreeComponent* SkillComp = OwnerCharacter->FindComponentByClass<USkillTreeComponent>())
	{
		const FSkillTreeDataRow* SkillData = SkillComp->FindSkillDataByGrantedAbility(GetClass());
		if (SkillData)
		{
			Damage = SkillData->SkillDamage;
		}
	}

	SpawnOrbsAndFire(OwnerCharacter, Damage);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_AutoRaser::SpawnOrbsAndFire(ACharacter* OwnerCharacter, float Damage)
{
	if (!OrbActorClass || !OwnerCharacter) return;

	UWorld* World = OwnerCharacter->GetWorld();
	if (!World) return;

	const FVector Origin = OwnerCharacter->GetActorLocation();
	const FVector Left = OwnerCharacter->GetActorRightVector()*-1.f;   // �߽� ���� �� (Y��)
	const FVector Up = OwnerCharacter->GetActorUpVector();         // ȸ�� �� (Z��)

	const int32 OrbCount = 4;
	const float Radius = OrbSpawnRadius;
	const float AngleStep = 180.f / (OrbCount - 1); // �ݿ� ���� ����

	for (int32 i = 0; i < OrbCount; ++i)
	{
		const float AngleDeg = -90.f + (AngleStep * i); // -90 ~ +90
		const float AngleRad = FMath::DegreesToRadians(AngleDeg);

		// X�� ���� �ݿ� �� YZ ��鿡 �ݿ�
		FVector Offset =
			FMath::Cos(AngleRad) * Up +   // �߽� �� (Y��)
			FMath::Sin(AngleRad) * Left;       // ���Ʒ� ����

		FVector SpawnLocation = Origin + Offset * Radius;
		FRotator SpawnRotation = Offset.Rotation();

		ARaserOrb* Orb = World->SpawnActor<ARaserOrb>(OrbActorClass, SpawnLocation, SpawnRotation);
		if (Orb)
		{
			Orb->Initialize(OwnerCharacter, Damage);

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