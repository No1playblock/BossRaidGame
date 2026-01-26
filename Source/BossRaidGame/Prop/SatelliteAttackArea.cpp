// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/SatelliteAttackArea.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "SubSystems/ObjectPoolSubsystem.h"
#include "Tag/BRGameplayTag.h" // 데미지 태그
#include "Engine/OverlapResult.h"
#include "SubSystems/EffectPoolSubsystem.h" 

ASatelliteAttackArea::ASatelliteAttackArea()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(RootComp);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 쿼리용으로만 사용

}

void ASatelliteAttackArea::OnPoolSpawned()
{
	// 초기화 로직 (필요시)
	//AttackRadius = 200.0f;

	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASatelliteAttackArea::OnPoolDespawned()
{
	// 정리 로직
	GetWorld()->GetTimerManager().ClearTimer(ReturnTimerHandle);
	//ParticleComp->DeactivateSystem();

	AttackRadius = SphereRadius;	//이거 AttackRange 고치자.
	DamageAmount = 0.0f;
	DamageEffectClass = nullptr;

	//AimingParticleComp->DeactivateSystem();
}

void ASatelliteAttackArea::InitializeInfo_Implementation(float Damage, TSubclassOf<UGameplayEffect> DamageEffect)
{
	DamageAmount = Damage;
	DamageEffectClass = DamageEffect;

	SetOwner(GetInstigator());

	AttackRadius = SphereRadius;

	StartAiming();
}
void ASatelliteAttackArea::StartAiming()
{
	SphereComp->SetSphereRadius(AttackRadius);

	if (AimingVFX)
	{
		if (UWorld* World = GetWorld())
		{
			// 이펙트 풀 서브시스템 가져오기
			if (UEffectPoolSubsystem* EffectPool = World->GetSubsystem<UEffectPoolSubsystem>())
			{
				// 위치는 현재 액터 위치, 회전은 기본값, 스케일 1.0
				AimingParticleComp= EffectPool->SpawnEffect(AimingVFX, GetActorLocation(), FRotator::ZeroRotator, FVector(AimingVFXScale));
			}
			else
			{
				// 혹시라도 풀 시스템이 없으면 일반 스폰 (안전장치)
				AimingParticleComp = UGameplayStatics::SpawnEmitterAtLocation(World, AimingVFX, GetActorLocation());
			}
		}
	}


}
void ASatelliteAttackArea::TriggerSkill_Implementation()
{
	ExecuteExplosion();
}
void ASatelliteAttackArea::ExecuteExplosion()
{
	if (AimingParticleComp)
	{
		AimingParticleComp->DeactivateSystem();
		AimingParticleComp = nullptr;
	}
	if (ExplosionVFX)
	{
		if (UWorld* World = GetWorld())
		{
			// 이펙트 풀 서브시스템 가져오기
			if (UEffectPoolSubsystem* EffectPool = World->GetSubsystem<UEffectPoolSubsystem>())
			{
				// 위치는 현재 액터 위치, 회전은 기본값, 스케일 1.0
				EffectPool->SpawnEffect(ExplosionVFX, GetActorLocation(), FRotator::ZeroRotator, FVector(ExplosionScale));
			}
			else
			{
				// 혹시라도 풀 시스템이 없으면 일반 스폰 (안전장치)
				UGameplayStatics::SpawnEmitterAtLocation(World, ExplosionVFX, GetActorLocation());
			}
		}
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	if (DrawDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 12, FColor::Red, false, 1.0f);

	}

	UE_LOG(LogTemp, Warning, TEXT("SatelliteAttackArea: Executing Explosion at Location: %s with Radius: %f"), *GetActorLocation().ToString(), AttackRadius);
	//범위 내 적 감지
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner()); // 시전자는 제외

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn); // 폰만 감지

	bool bHit = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	// 디버그 (필요시 주석 해제)
	UE_LOG(LogTemp, Log, TEXT("SatelliteAttackArea: OverlapMultiByObjectType returned %s"), bHit ? TEXT("true") : TEXT("false"));
	// 데미지 적용
	if (bHit)
	{
		TSet<AActor*> AlreadyHitActors;
		for (const FOverlapResult& Result : OverlapResults)
		{
			if (AActor* OverlappedActor = Result.GetActor())
			{
				if (OverlappedActor && OverlappedActor != this && !AlreadyHitActors.Contains(OverlappedActor) && OverlappedActor != GetInstigator())
				{

					AlreadyHitActors.Add(OverlappedActor);
					if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlappedActor))
					{
						if (DamageEffectClass)
						{
							FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
							// 데미지 가해자를 이 발사체를 쏜 캐릭터(GetOwner())로 설정
							Context.AddInstigator(GetOwner(), this);

							FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.f, Context);
							if (SpecHandle.IsValid())
							{
								SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_DAMAGE, DamageAmount);

								TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
							}
						}
					}
				}
			}
		}
	}

	//이펙트가 끝날 때쯤 풀로 반환
	GetWorld()->GetTimerManager().SetTimer(ReturnTimerHandle, this, &ASatelliteAttackArea::ReturnPool, 2.0f, false);
}

void ASatelliteAttackArea::ReturnPool()
{
	if (UObjectPoolSubsystem* PoolSys = GetWorld()->GetSubsystem<UObjectPoolSubsystem>())
	{
		PoolSys->ReturnToPool(this);
	}
	else
	{
		Destroy();
	}
}

