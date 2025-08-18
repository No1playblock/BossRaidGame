// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_ShootProjectile.h"
#include "Prop/PrimaryBullet.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GASCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PlayerCharacterAttributeSet.h"

UAT_ShootProjectile* UAT_ShootProjectile::CreateTask(UGameplayAbility* OwningAbility, FName TaskName)
{
	return NewAbilityTask<UAT_ShootProjectile>(OwningAbility, TaskName);
}
void UAT_ShootProjectile::Activate()
{
	Super::Activate();
	DoShoot();
	EndTask();
}
void UAT_ShootProjectile::Initialize(TSubclassOf<class APrimaryBullet> InBulletClass, TSubclassOf<class UGameplayEffect> InAttackEffectClass)
{
	BulletClass = InBulletClass;
	AttackEffectClass = InAttackEffectClass;
}

void UAT_ShootProjectile::DoShoot()
{
	AActor* Avatar = GetAvatarActor();
	if (!Avatar || !BulletClass) return;

	USkeletalMeshComponent* Mesh = Avatar->FindComponentByClass<USkeletalMeshComponent>();
	FVector MuzzleLocation = Mesh->GetSocketLocation(TEXT("Socket_Barrel")); // ���� �̸� ����

	APlayerController* PC = Cast<APlayerController>(Avatar->GetInstigatorController());
	if (!PC) return;

	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);

	FVector CamWorldLoc, CamWorldDir;
	if (!PC->DeprojectScreenPositionToWorld(SizeX / 2, SizeY / 2, CamWorldLoc, CamWorldDir))
		return;
	FVector TraceStart = CamWorldLoc;
	FVector TraceEnd = TraceStart + CamWorldDir * 10000.f;

	FHitResult AimHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Avatar);

	bool bHit = GetWorld()->LineTraceSingleByChannel(AimHit, TraceStart, TraceEnd, ECC_Visibility, Params);

	FVector AimPoint = bHit ? AimHit.ImpactPoint : TraceEnd;

	FVector BulletDirection = (AimPoint - MuzzleLocation).GetSafeNormal();

	FRotator BulletRotation = BulletDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);

	AGASCharacterPlayer* BaseCharacter = Cast<AGASCharacterPlayer>(Avatar);
	if (BaseCharacter)
	{
		UAbilitySystemComponent* ASC = BaseCharacter->GetAbilitySystemComponent();
		if (ASC)
		{
			APrimaryBullet* Bullet = GetWorld()->SpawnActor<APrimaryBullet>(BulletClass, MuzzleLocation, BulletRotation, SpawnParams);
			Bullet->InitializeBullet(ASC->GetNumericAttribute(UPlayerCharacterAttributeSet::GetAttackPowerAttribute()), AttackEffectClass);
			//Bullet->SetDamage(ASC->GetNumericAttribute(UPlayerCharacterAttributeSet::GetAttackPowerAttribute()));
			UE_LOG(LogTemp, Warning, TEXT("AT_Shoot Damage: %f"), ASC->GetNumericAttribute(UPlayerCharacterAttributeSet::GetAttackPowerAttribute()));
		}
	}

	
}

