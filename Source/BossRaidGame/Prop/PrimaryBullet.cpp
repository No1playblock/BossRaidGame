// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/PrimaryBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Tag/BRGameplayTag.h"
#include "Subsystems/ObjectPoolSubsystem.h"

APrimaryBullet::APrimaryBullet()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	RootComponent = CollisionComp;

	CollisionComp->SetGenerateOverlapEvents(true);

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(RootComponent);



	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;


}
void APrimaryBullet::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APrimaryBullet::OnBeginOverlap);

}

void APrimaryBullet::OnPoolSpawned()
{
	SetActorHiddenInGame(false);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 물리 이동 초기화 및 재시작
	ProjectileMovement->SetUpdatedComponent(CollisionComp);
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
	ProjectileMovement->UpdateComponentVelocity(); // 속도 갱신 강제
	ProjectileMovement->Activate(true);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(DeactivateTimerHandle, this, &APrimaryBullet::ReturnToPool, 3.0f, false);
	}
}

void APrimaryBullet::OnPoolDespawned()
{
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Deactivate();

	// 충돌 판정 끄기
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DeactivateTimerHandle);
	}

}
void APrimaryBullet::InitializeBullet(float InDamage, TSubclassOf<class UGameplayEffect> InDamageEffectClass)
{
	Damage = InDamage;
	DamageEffectClass = InDamageEffectClass;
}
void APrimaryBullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			if (DamageEffectClass)
			{
				FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
				// 데미지 가해자를 이 발사체를 쏜 캐릭터(GetOwner())로 설정
				Context.AddInstigator(GetOwner(), this);

				FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.f, Context);
				if (SpecHandle.IsValid())
				{
					SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_DAMAGE, Damage);

					TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
		ReturnToPool();
	}
}

void APrimaryBullet::ReturnToPool()
{
	if (UWorld* World = GetWorld())
	{
		if (UObjectPoolSubsystem* PoolSys = World->GetSubsystem<UObjectPoolSubsystem>())
		{
			// 타이머 안전하게 정리 후 반환
			World->GetTimerManager().ClearTimer(DeactivateTimerHandle);
			PoolSys->ReturnToPool(this);
		}
		else
		{
			Destroy();
		}
	}
}
