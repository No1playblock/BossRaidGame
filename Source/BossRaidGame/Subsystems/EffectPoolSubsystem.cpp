// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/EffectPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"


void UEffectPoolSubsystem::WarmUpPool(UParticleSystem* EffectTemplate, int32 Count)
{
	if (!EffectTemplate || !GetWorld()) return;

	for (int32 i = 0; i < Count; i++)
	{
		// °­Á¦·Î ÇÏ³ª »ý¼º
		UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			EffectTemplate,
			FVector(0, 0, -5000), // ¶¥ ¹Ø ±í¼÷ÇÑ °÷
			FRotator::ZeroRotator,
			FVector(1),
			false, // bAutoDestroy = false (ÇÊ¼ö)
			EPSCPoolMethod::None,
			true
		);

		if (PSC)
		{
			// ¹Ù·Î ²ô°í ¼û±è
			PSC->DeactivateSystem();
			PSC->SetHiddenInGame(true);

			//µ¨¸®°ÔÀÌÆ® ¿¬°á 
			PSC->OnSystemFinished.AddDynamic(this, &UEffectPoolSubsystem::OnParticleSystemFinished);

			EffectPool.FindOrAdd(EffectTemplate).Items.Add(PSC);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Effect Pool Warmed Up: %s (%d created)"), *EffectTemplate->GetName(), Count);
}

UParticleSystemComponent* UEffectPoolSubsystem::SpawnEffect(UParticleSystem* EffectTemplate, FVector Location, FRotator Rotation, FVector Scale)
{
	if (!EffectTemplate || !GetWorld()) return nullptr;

	UParticleSystemComponent* SelectedPSC = nullptr;

	// Ç® °Ë»ç
	if (EffectPool.Contains(EffectTemplate) && EffectPool[EffectTemplate].Items.Num() > 0)
	{
		// Items¿¡¼­ ²¨³¿ 
		SelectedPSC = EffectPool[EffectTemplate].Items.Pop();

		UE_LOG(LogTemp, Log, TEXT("Effect Pool: Reusing existing particle system for %s"), *EffectTemplate->GetName());
		if (IsValid(SelectedPSC))
		{
			SelectedPSC->SetWorldLocationAndRotation(Location, Rotation);
			SelectedPSC->SetWorldScale3D(Scale);
			SelectedPSC->SetHiddenInGame(false);
			SelectedPSC->ActivateSystem(true);
			SelectedPSC->ReregisterComponent();

			return SelectedPSC;
		}
	}

	//¾øÀ¸¸Ç »õ·Î »ý¼º
	SelectedPSC = UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		EffectTemplate,
		Location,
		Rotation,
		Scale,
		false,
		EPSCPoolMethod::None,
		true
	);

	if (SelectedPSC)
	{
		SelectedPSC->OnSystemFinished.AddDynamic(this, &UEffectPoolSubsystem::OnParticleSystemFinished);
	}

	return SelectedPSC;
}



void UEffectPoolSubsystem::OnParticleSystemFinished(UParticleSystemComponent* PSC)
{
	if (!IsValid(PSC)) return;

	PSC->DeactivateSystem();
	PSC->SetHiddenInGame(true);

	UParticleSystem* Template = PSC->Template;
	if (Template)
	{
		EffectPool.FindOrAdd(Template).Items.Add(PSC);
	}
}
