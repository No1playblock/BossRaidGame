// Fill out your copyright notice in the Description page of Project Settings.


#include "StoneStatue.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Character/GASCharacterPlayer.h"
#include "Blueprint/UserWidget.h" // CreateWidget ���
#include "Components/WidgetComponent.h"
#include "AbilitySystemComponent.h" // ASC ����� ���� ����
#include "AbilitySystemBlueprintLibrary.h" // �̺�Ʈ ������ ���� ����
#include "Animation/AnimInstance.h" // AnimInstance ����� ���� �߰�

// Sets default values
AStoneStatue::AStoneStatue()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = BoxComponent;

	StatueMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StatueMesh"));
	StatueAnimalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StatueAnimalMesh"));

	//StatueMesh->SetCollisionProfileName(TEXT("NoCollision")); // �浹 ������ ����
	//StatueAnimalMesh->SetCollisionProfileName(TEXT("NoCollision")); // �浹 ������ ����

	StatueMesh->SetCollisionProfileName(TEXT("BlockAll"));
	StatueAnimalMesh->SetCollisionProfileName(TEXT("BlockAll"));

	StatueMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No; // ĳ���Ͱ� �� �޽ÿ� �ö� �� ������ ����

	StatueMesh->SetupAttachment(RootComponent);
	StatueAnimalMesh->SetupAttachment(StatueMesh);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AStoneStatue::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AStoneStatue::OnOverlapEnd);

	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidgetComponent"));
	InteractionWidgetComponent->SetupAttachment(RootComponent);
	InteractionWidgetComponent->SetVisibility(false); // �⺻������ ������ ������ �ʵ��� ����
	InteractionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractionWidgetComponent->SetCollisionProfileName(TEXT("NoCollision"));
	InteractionWidgetComponent->SetDrawSize(FVector2D(200.0f, 50.0f)); // ���� ũ�� ����
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // Screen Space�� �����ϸ� �׻� ī�޶� �ٶ�
	InteractionWidgetComponent->SetWidgetClass(InteractionWidgetClass); // ���� Ŭ���� ����
}

void AStoneStatue::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(OtherActor))
	{
		// �÷��̾�� ��ȣ�ۿ� ������ ���Ͱ� '��'��� �˷���
		PlayerCharacter->SetInteractableActor(this);
		
		if (StatueMesh)
		{
			StatueMesh->SetRenderCustomDepth(true); // �ܰ��� �ѱ�
		}
		if (InteractionWidgetComponent)
		{
			InteractionWidgetComponent->SetVisibility(true); // ���� ���̱�

		}
	}
}

void AStoneStatue::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(OtherActor))
	{
		// �÷��̾�� ��ȣ�ۿ� �׼��� �����϶�� �˸�
		PlayerCharacter->ClearInteractableActor(this);
		// ��Ŀ�� ���� (UI ���� �� �ܰ���)
		if (StatueMesh)
		{
			StatueMesh->SetRenderCustomDepth(false); // �ܰ��� ����
		}
		if (InteractionWidgetComponent)
		{
			InteractionWidgetComponent->SetVisibility(false); // ���� �����

		}
	}
	
}
void AStoneStatue::Interact(AActor* InstigatorActor)
{
	if (InteractionEventTag.IsValid())
	{
		if (DisableMontage && StatueAnimalMesh)
		{
			UAnimInstance* AnimInstance = StatueAnimalMesh->GetAnimInstance();
			if (AnimInstance)
			{
				// ��Ÿ�� ���
				AnimInstance->Montage_Play(DisableMontage);
			}
		}

		FGameplayEventData Payload;
		Payload.Instigator = InstigatorActor;
		Payload.Target = this;

		// UPROPERTY ������ ����Ͽ� �̺�Ʈ�� �����մϴ�.
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstigatorActor, InteractionEventTag, Payload);
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionEventTag is not set on %s"), *GetName());
	}

	// ��ȣ�ۿ� �� �ߺ� ���� ����
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (InteractionWidgetComponent) InteractionWidgetComponent->SetVisibility(false);
	if (StatueMesh) StatueMesh->SetRenderCustomDepth(false);
}
	