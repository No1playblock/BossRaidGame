// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/HealingPool.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AHealingPool::AHealingPool()
{
	PrimaryActorTick.bCanEverTick = false;

	// 스피어 컴포넌트 생성 및 설정
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("SphereComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 모든 다이내믹 액터와 오버랩되도록 설정

	// 파티클 시스템 컴포넌트 생성 및 루트에 부착
	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(RootComponent);
}

void AHealingPool::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(PoolLifetime);

	GetWorld()->GetTimerManager().SetTimer(HealingTimerHandle, this, &AHealingPool::ApplyHealing, HealingInterval, true);
}

void AHealingPool::ApplyHealing()
{
	if (!HealingEffectClass)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors, TSubclassOf<APawn>()); // Pawn 클래스만 필터링

	for (AActor* OverlappedActor : OverlappingActors)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlappedActor);
		if (TargetASC)
		{
			FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(HealingEffectClass, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}


