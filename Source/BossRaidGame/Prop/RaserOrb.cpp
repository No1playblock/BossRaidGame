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

void ARaserOrb::Initialize(AActor* InInstigator, float InDamage)
{
	InstigatorActor = InInstigator;
	DamageAmount = InDamage;
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

		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, WorldOrigin, TraceEnd, ECC_Pawn, Params);

		DrawDebugLine(GetWorld(), GetActorLocation(), bHit ? Hit.Location : TraceEnd, bHit ? FColor::Green : FColor::Red, false, 1.0f, 0, 1.0f);

		return Hit;
	}

	return FHitResult();
}

void ARaserOrb::SelfDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("RaserOrb SelfDestroy called"));

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

				// 2. 플레이어(Instigator)의 SkillPower 어트리뷰트 값을 가져옵니다.
				const float SkillPower = SourceASC->GetNumericAttribute(UPlayerCharacterAttributeSet::GetSkillPowerAttribute());

				// 3. 최종 데미지를 계산합니다. SkillPower가 0이면 데미지가 없으므로, 1을 기본값으로 간주할 수 있습니다.
				const float FinalDamage = BaseDamage * FMath::Max(1.f, SkillPower);
				UE_LOG(LogTemp, Warning, TEXT("Final Damage: %f"), FinalDamage);
				if (FinalDamage > 0.f)
				{
					if (DamageEffectClass)
					{
						FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
						ContextHandle.AddSourceObject(this);

						FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ContextHandle);
						if (SpecHandle.IsValid())
						{
							SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_DAMAGE_QSKILL, -1.0f * FinalDamage);		//여기서 음수로 줌
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