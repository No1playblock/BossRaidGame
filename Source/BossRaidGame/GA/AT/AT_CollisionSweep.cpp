// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_CollisionSweep.h"
#include "Character/GASCharacterPlayer.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AT_CollisionSweep.h"
#include "GA/GA_AttackHitCheck.h"


UAT_CollisionSweep::UAT_CollisionSweep()
{

}

UAT_CollisionSweep* UAT_CollisionSweep::CreateTask(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UAT_CollisionSweep* NewTask = NewAbilityTask<UAT_CollisionSweep>(OwningAbility);
	return NewTask;
}

void UAT_CollisionSweep::Activate()
{
	Super::Activate();
	PerformSweep();

	GetWorld()->GetTimerManager().SetTimer(SweepTimerHandle, this, &UAT_CollisionSweep::PerformSweep, 0.011f, true); // 30fps 기준 반복
}

void UAT_CollisionSweep::OnDestroy(bool AbilityEnded)
{
	GetWorld()->GetTimerManager().ClearTimer(SweepTimerHandle);
	Super::OnDestroy(AbilityEnded);

}

void UAT_CollisionSweep::PerformSweep()
{
	//UE_LOG(LogTemp, Warning, TEXT("PerformSweep"));

	AGASCharacterPlayer* Character = Cast<AGASCharacterPlayer>(GetAvatarActor());
	if (!Character) return;

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh) return;
	const FName SocketName = TEXT("root_weaponSocket"); // 사용할 Socket 이름
	const FName SocketEndName = TEXT("root_weaponEndSocket"); // 사용할 Socket 이름
	FVector EndLocation = Mesh->GetSocketLocation(SocketEndName);
	
	FVector Location = Mesh->GetSocketLocation(SocketName);
	FQuat Rotation = Mesh->GetSocketQuaternion(SocketName);

	FVector BoxExtent = FVector(6.f, 105.f, 13.f); // 원하는 판정 범위

	if (!bHasLastLocation)
	{
		LastLocation = Location;
		EndLastLocation = EndLocation;
		bHasLastLocation = true;
		return;
	}

	FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxExtent);
	TArray<FHitResult> Hits;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		LastLocation,
		Location, 
		Rotation,
		ECollisionChannel::ECC_Pawn, // 설정한 채널 사용
		BoxShape,
		Params
	);

#if ENABLE_DRAW_DEBUG


	FColor DrawColor = bHit ? FColor::Green : FColor::Red;
	//DrawDebugBox(GetWorld(), Location, BoxExtent, Rotation, DrawColor, false, 2.0f);
	DrawDebugLine(GetWorld(), EndLastLocation, EndLocation, DrawColor, false, 1.0f, 0, 1.f);

#endif
	LastLocation = Location;
	EndLastLocation = EndLocation;

	TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;
	if (const auto* GA = Cast<UGA_AttackHitCheck>(Ability))
	{
		DamageEffectClass = GA->GetDamageEffectClass();
	}

	if (!DamageEffectClass) return;

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == GetAvatarActor()) continue;

		if (HitActors.Contains(HitActor)) continue;

		HitActors.Add(HitActor); // 최초 1회만 통과

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
		if (!TargetASC) continue;

		UAbilitySystemComponent* SourceASC = Ability->GetAbilitySystemComponentFromActorInfo();
		if (!SourceASC) return;

		FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, EffectContext);

		if (SpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
}
