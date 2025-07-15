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

	// 설정된 수명(PoolLifetime) 이후에 액터가 자동으로 파괴되도록 설정합니다.
	SetLifeSpan(PoolLifetime);

	// 설정된 간격(HealingInterval)마다 ApplyHealing 함수를 반복적으로 호출하는 타이머를 설정합니다.
	GetWorld()->GetTimerManager().SetTimer(HealingTimerHandle, this, &AHealingPool::ApplyHealing, HealingInterval, true);
}

void AHealingPool::ApplyHealing()
{
	if (!HealingEffectClass)
	{
		return;
	}

	// 현재 스피어 컴포넌트와 오버랩 중인 모든 액터를 가져옵니다.
	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors, TSubclassOf<APawn>()); // Pawn 클래스만 필터링

	for (AActor* OverlappedActor : OverlappingActors)
	{
		// 오버랩된 액터로부터 AbilitySystemComponent를 가져옵니다.
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlappedActor);
		if (TargetASC)
		{
			// 이펙트 컨텍스트를 만들고, 이 힐링 장판(this)을 효과의 근원(SourceObject)으로 지정합니다.
			FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			// 이펙트 스펙을 생성합니다.
			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(HealingEffectClass, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				// 대상에게 힐링 이펙트를 적용합니다.
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}


