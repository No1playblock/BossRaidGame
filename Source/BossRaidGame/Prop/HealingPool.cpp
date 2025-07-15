// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/HealingPool.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AHealingPool::AHealingPool()
{
	PrimaryActorTick.bCanEverTick = false;

	// ���Ǿ� ������Ʈ ���� �� ����
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("SphereComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // ��� ���̳��� ���Ϳ� �������ǵ��� ����

	// ��ƼŬ �ý��� ������Ʈ ���� �� ��Ʈ�� ����
	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(RootComponent);
}

void AHealingPool::BeginPlay()
{
	Super::BeginPlay();

	// ������ ����(PoolLifetime) ���Ŀ� ���Ͱ� �ڵ����� �ı��ǵ��� �����մϴ�.
	SetLifeSpan(PoolLifetime);

	// ������ ����(HealingInterval)���� ApplyHealing �Լ��� �ݺ������� ȣ���ϴ� Ÿ�̸Ӹ� �����մϴ�.
	GetWorld()->GetTimerManager().SetTimer(HealingTimerHandle, this, &AHealingPool::ApplyHealing, HealingInterval, true);
}

void AHealingPool::ApplyHealing()
{
	if (!HealingEffectClass)
	{
		return;
	}

	// ���� ���Ǿ� ������Ʈ�� ������ ���� ��� ���͸� �����ɴϴ�.
	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors, TSubclassOf<APawn>()); // Pawn Ŭ������ ���͸�

	for (AActor* OverlappedActor : OverlappingActors)
	{
		// �������� ���ͷκ��� AbilitySystemComponent�� �����ɴϴ�.
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlappedActor);
		if (TargetASC)
		{
			// ����Ʈ ���ؽ�Ʈ�� �����, �� ���� ����(this)�� ȿ���� �ٿ�(SourceObject)���� �����մϴ�.
			FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			// ����Ʈ ������ �����մϴ�.
			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(HealingEffectClass, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				// ��󿡰� ���� ����Ʈ�� �����մϴ�.
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}


