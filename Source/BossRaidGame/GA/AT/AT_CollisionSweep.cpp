// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_CollisionSweep.h"
#include "Character/GASCharacterPlayer.h"
#include "AT_CollisionSweep.h"


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

	GetWorld()->GetTimerManager().SetTimer(SweepTimerHandle, this, &UAT_CollisionSweep::PerformSweep, 0.001f, true); // 30fps 기준 반복
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

	FVector Location = Mesh->GetSocketLocation(SocketName);
	FQuat Rotation = Mesh->GetSocketQuaternion(SocketName);
	FTransform SocketLocalTransform = Mesh->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_Actor);
	FVector BoxExtent = FVector(6.f, 105.f, 13.f); // 원하는 판정 범위


	FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxExtent);
	TArray<FHitResult> Hits;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		Location,
		Location, // Static sweep (이동 없음)
		Rotation,
		ECollisionChannel::ECC_Pawn, // 설정한 채널 사용
		BoxShape,
		Params
	);

#if ENABLE_DRAW_DEBUG


	FColor DrawColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugBox(GetWorld(), Location, BoxExtent, Rotation, DrawColor, false, 2.0f);


#endif


	for (const FHitResult& Hit : Hits)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			// 충돌 처리 로직
			UE_LOG(LogTemp, Log, TEXT("Hit actor: %s"), *HitActor->GetName());
		}
	}
}
