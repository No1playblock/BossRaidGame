// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/PrimaryBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Tag/BRGameplayTag.h"

APrimaryBullet::APrimaryBullet()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	//CollisionComp->OnComponentHit.AddDynamic(this, &APrimaryBullet::OnHit);
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

	
	//InitialLifeSpan = 3.0f;

}
void APrimaryBullet::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APrimaryBullet::OnBeginOverlap);

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
		
		//UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *OtherActor->GetFName().ToString());
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
		Destroy();
	}
}