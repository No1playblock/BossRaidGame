// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/RaserOrb.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"

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
	Destroy();
}

void ARaserOrb::FireLaser()
{
	

	FHitResult Hit = LineTrace();


	if (Hit.IsValidBlockingHit())
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor))
			{
				if (DamageEffectClass)
				{
					FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
					ContextHandle.AddSourceObject(this);

					FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ContextHandle);
					if (SpecHandle.IsValid())
					{
						SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage"), DamageAmount);
						TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
					}
				}
			}
		}
	}

	FTimerHandle DestroyTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ARaserOrb::SelfDestroy, 1.0f, false);
}