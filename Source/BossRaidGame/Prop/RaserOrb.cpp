// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/RaserOrb.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "Attribute/PlayerCharacterAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Tag/BRGameplayTag.h"

// Sets default values
ARaserOrb::ARaserOrb()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}
void ARaserOrb::BeginPlay()
{
	Super::BeginPlay();
}

void ARaserOrb::Initialize(AActor* InInstigator, float InDamage, TSubclassOf<UGameplayEffect> NewDamageEffectClass, TEnumAsByte<ECollisionChannel> NewTargetChannel)
{
	InstigatorActor = InInstigator;
	DamageAmount = InDamage;
	DamageEffectClass = NewDamageEffectClass;
	TargetChannel = NewTargetChannel;
}

FHitResult ARaserOrb::LineTrace() const
{
	if (!InstigatorActor) return FHitResult();

	APawn* InstigatorPawn = Cast<APawn>(InstigatorActor);
	if (!InstigatorPawn) return FHitResult();

	APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController());
	if (!PC) return FHitResult();

	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);

	FVector WorldOrigin;
	FVector WorldDirection;
	if (PC->DeprojectScreenPositionToWorld(SizeX / 2.0f, SizeY / 2.0f, WorldOrigin, WorldDirection))
	{
		FVector TraceEnd = WorldOrigin + WorldDirection * 10000.f;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(InstigatorActor);

		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, WorldOrigin, TraceEnd, TargetChannel, Params);

		DrawDebugLine(GetWorld(), GetActorLocation(), bHit ? Hit.Location : TraceEnd, bHit ? FColor::Green : FColor::Red, false, 1.0f, 0, 1.0f);

		return Hit;
	}

	return FHitResult();
}

void ARaserOrb::SelfDestroy()
{
	Destroy();
}

void ARaserOrb::FireLaser()
{


	FHitResult Hit = LineTrace();


	if (Hit.IsValidBlockingHit())
	{
		AActor* HitActor = Hit.GetActor();

		if (HitActor && InstigatorActor && DamageEffectClass)
		{
			UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor);
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

			if (SourceASC && TargetASC)
			{

				const float BaseDamage = DamageAmount;

				//�÷��̾��� SkillPower ��Ʈ����Ʈ ��
				const float SkillPower = SourceASC->GetNumericAttribute(UPlayerCharacterAttributeSet::GetSkillPowerAttribute());

				//���� �������� ���,  SkillPower�� 0�̸� �������� �����Ƿ�, 1�� �⺻������ ����
				//const float FinalDamage = BaseDamage * FMath::Max(1.f, SkillPower);
				//UE_LOG(LogTemp, Warning, TEXT("Final Damage: %f"), FinalDamage);
				if (BaseDamage > 0.f)
				{
					if (DamageEffectClass)
					{
						FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
						ContextHandle.AddSourceObject(this);
						ContextHandle.AddInstigator(InstigatorActor, this);
						FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ContextHandle);
						if (SpecHandle.IsValid())
						{
							SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_DAMAGE, BaseDamage);
							TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
						}
					}
				}
			}
		}

	}


	FTimerHandle DestroyTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ARaserOrb::SelfDestroy, 1.0f, false);
}