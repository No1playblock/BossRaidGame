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

	GetWorld()->GetTimerManager().SetTimer(SweepTimerHandle, this, &UAT_CollisionSweep::PerformSweep, 0.033f, true); // 30fps ���� �ݺ�
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
    const FName SocketName = TEXT("root_weapon"); // ����� Socket �̸�
    FVector Location = Mesh->GetSocketLocation(SocketName);
    
    FQuat Rotation = Mesh->GetSocketQuaternion(SocketName);
    FVector BoxExtent = FVector(6.f, 105.f, 13.f); // ���ϴ� ���� ����
    Location.Y += (BoxExtent.Y / 2);

    FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxExtent);
    TArray<FHitResult> Hits;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Character);

    bool bHit = GetWorld()->SweepMultiByChannel(
        Hits,
        Location,
        Location, // Static sweep (�̵� ����)
        Rotation,
        ECC_GameTraceChannel1, // ������ ä�� ���
        BoxShape,
        Params
    );
#if ENABLE_DRAW_DEBUG

    if (bHit)
    {
        FColor DrawColor = bHit ? FColor::Green : FColor::Red;
        DrawDebugBox(GetWorld(), Location, BoxExtent, DrawColor, false, 2.0f);
    }

#endif


    for (const FHitResult& Hit : Hits)
    {
        if (AActor* HitActor = Hit.GetActor())
        {
            // �浹 ó�� ����
            //UE_LOG(LogTemp, Log, TEXT("Hit actor: %s"), *HitActor->GetName());
        }
    }
}
