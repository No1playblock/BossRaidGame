// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/Grenade.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Tag/BRGameplayTag.h"
#include "Components/SkillTreeComponent.h"
#include "Engine/OverlapResult.h"
#include "Subsystems/EffectPoolSubsystem.h"
#include "Subsystems/ObjectPoolSubsystem.h"
// Sets default values
AGrenade::AGrenade()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;

	SphereComp->InitSphereRadius(30.0f);
	SphereComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AGrenade::OnBeginOverlap);
	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	GrenadeMesh->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = SphereComp;

	ProjectileMovement->InitialSpeed = 200;
	ProjectileMovement->MaxSpeed = 10000.f;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 1.0f;

}

void AGrenade::OnPoolSpawned()
{

	FVector LaunchDirection = GetActorForwardVector();

	if (APawn* MyInstigator = GetInstigator())
	{
		if (APlayerController* PC = Cast<APlayerController>(MyInstigator->GetController()))
		{
			FVector CamLoc;
			FRotator CamRot;
			PC->GetPlayerViewPoint(CamLoc, CamRot);

			// 화면 중앙으로 레이캐스트
			FVector TraceStart = CamLoc;
			FVector TraceEnd = CamLoc + (CamRot.Vector() * 50000.0f); // 500미터 앞

			FHitResult HitResult;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);          // 나 자신 무시
			Params.AddIgnoredActor(MyInstigator);  // 주인 무시

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				TraceStart,
				TraceEnd,
				ECollisionChannel::ECC_Visibility,
				Params
			);

			FVector TargetLocation = bHit ? HitResult.Location : TraceEnd;

			// 방향 벡터
			LaunchDirection = (TargetLocation - GetActorLocation()).GetSafeNormal();
		}
		else
		{
			LaunchDirection = MyInstigator->GetActorForwardVector();
		}
	}

	//수류탄 자체를 그 방향으로 회전
	//SetActorRotation(LaunchDirection.Rotation());

	//발사 속도 적용
	ProjectileMovement->Velocity = LaunchDirection * VelocityStrength * 1500.0f;

	ProjectileMovement->Activate();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ReturnTimerHandle, this, &AGrenade::ReturnSelf, 3.0f, false);
	}
}

void AGrenade::OnPoolDespawned()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ReturnTimerHandle);
	}
	ProjectileMovement->Deactivate();
}

void AGrenade::BeginPlay()
{
	Super::BeginPlay();
}
void AGrenade::InitializeInfo_Implementation(float InDamage, TSubclassOf<UGameplayEffect> InDamageEffect)
{
	// 여기서 받아온 값을 내 변수에 저장
	this->Damage = InDamage;
	this->DamageEffectClass = InDamageEffect;

}
void AGrenade::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetInstigator())
	{
		UE_LOG(LogTemp, Warning, TEXT("Instiagtror: %s"), *GetInstigator()->GetName());

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Instigator"));
	}
	if (OtherActor && OtherActor != this && OtherActor != GetInstigator())
	{
		UWorld* World = GetWorld();
		if (!World) return;
		if (ExplosionEffect)
		{

			if (UEffectPoolSubsystem* PoolSys = World->GetSubsystem<UEffectPoolSubsystem>())
			{
				//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), FRotator::ZeroRotator, FVector(2.75f, 2.75f, 2.75f), true);
				PoolSys->SpawnEffect(ExplosionEffect, GetActorLocation(), FRotator::ZeroRotator, FVector(2.75));
			}
		}

		if (ExplosionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
		}

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		TArray<FOverlapResult> OverlapResults;

		bool bResult = GetWorld()->OverlapMultiByObjectType(
			OverlapResults,
			GetActorLocation(),
			FQuat::Identity, // 회전 없음
			ObjectParams,
			FCollisionShape::MakeSphere(300.f),
			Params
		);

		//DrawDebugSphere(GetWorld(), GetActorLocation(), 300.f, 24, FColor::Green, false, 2.0f);

		if (bResult)
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
									SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_DAMAGE, Damage);

									TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
								}
							}
						}
					}
				}
			}
		}

		if (UObjectPoolSubsystem* OPS = World->GetSubsystem<UObjectPoolSubsystem>())
		{
			OPS->ReturnToPool(this);
			
		}
		else
		{
			// 만약 서브시스템을 찾지 못했다면 안전하게 파괴
			Destroy();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Grenade overlapped with self or instigator."));
	}
}

void AGrenade::ReturnSelf()
{
	UE_LOG(LogTemp, Warning, TEXT("Grenade ReturnSelf called"));
	
	//이중으로 확인
	if (IsHidden())
	{
		return;
	}
	// 반환
	if (UWorld* World = GetWorld())
	{
		if (UObjectPoolSubsystem* PoolSys = World->GetSubsystem<UObjectPoolSubsystem>())
		{
			PoolSys->ReturnToPool(this);
		}
		else
		{
			Destroy(); // 서브시스템 없으면 그냥 파괴
		}
	}
}