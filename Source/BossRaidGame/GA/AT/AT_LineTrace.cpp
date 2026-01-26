// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_LineTrace.h"

#include "GameFramework/Actor.h"
#include "Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

UAT_LineTrace::UAT_LineTrace()
{

}

UAT_LineTrace* UAT_LineTrace::CreateTask(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UAT_LineTrace* Task = NewAbilityTask<UAT_LineTrace>(OwningAbility, TaskInstanceName);
	return Task;
}
void UAT_LineTrace::Activate()
{
	Super::Activate();
	DoShoot();
	EndTask();
}
void UAT_LineTrace::DoShoot()
{
	AActor* Avatar = GetAvatarActor();
	if (!Avatar) return;

	ABaseCharacter* Character = Cast<ABaseCharacter>(Avatar);
	if (!Character) return;

	APlayerController* PC = Cast<APlayerController>(Avatar->GetInstigatorController());
	if (!PC) return;

	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);

	FVector WorldOrigin, WorldDirection;
	PC->DeprojectScreenPositionToWorld(SizeX / 2.0f, SizeY / 2.0f, WorldOrigin, WorldDirection);

	const float TraceLength = 10000.f;
	FVector Start = WorldOrigin;
	FVector End = Start + WorldDirection * TraceLength;

	FVector SocketStart = Character->GetMesh()->GetSocketLocation(TEXT("Socket_Barrel"));
	FHitResult Hit;
	//Params.AddIgnoredActor(Avatar);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn);

	if (bHit)
	{
		OnHit.Broadcast(Hit);
	}
}

