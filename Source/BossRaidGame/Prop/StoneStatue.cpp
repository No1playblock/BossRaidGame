// Fill out your copyright notice in the Description page of Project Settings.


#include "StoneStatue.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Character/GASCharacterPlayer.h"
#include "Blueprint/UserWidget.h" // CreateWidget 사용
#include "Components/WidgetComponent.h"
#include "AbilitySystemComponent.h" // ASC 사용을 위해 포함
#include "AbilitySystemBlueprintLibrary.h" // 이벤트 전송을 위해 포함
#include "Animation/AnimInstance.h" // AnimInstance 사용을 위해 추가

// Sets default values
AStoneStatue::AStoneStatue()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = BoxComponent;

	StatueMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StatueMesh"));
	StatueAnimalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StatueAnimalMesh"));

	//StatueMesh->SetCollisionProfileName(TEXT("NoCollision")); // 충돌 프로필 설정
	//StatueAnimalMesh->SetCollisionProfileName(TEXT("NoCollision")); // 충돌 프로필 설정

	StatueMesh->SetCollisionProfileName(TEXT("BlockAll"));
	StatueAnimalMesh->SetCollisionProfileName(TEXT("BlockAll"));

	StatueMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No; // 캐릭터가 이 메시에 올라갈 수 없도록 설정

	StatueMesh->SetupAttachment(RootComponent);
	StatueAnimalMesh->SetupAttachment(StatueMesh);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AStoneStatue::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AStoneStatue::OnOverlapEnd);

	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidgetComponent"));
	InteractionWidgetComponent->SetupAttachment(RootComponent);
	InteractionWidgetComponent->SetVisibility(false); // 기본적으로 위젯은 보이지 않도록 설정
	InteractionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractionWidgetComponent->SetCollisionProfileName(TEXT("NoCollision"));
	InteractionWidgetComponent->SetDrawSize(FVector2D(200.0f, 50.0f)); // 위젯 크기 설정
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // Screen Space로 설정하면 항상 카메라를 바라봄
	InteractionWidgetComponent->SetWidgetClass(InteractionWidgetClass); // 위젯 클래스 설정
}

void AStoneStatue::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(OtherActor))
	{
		// 플레이어에게 상호작용 가능한 액터가 '나'라고 알려줌
		PlayerCharacter->SetInteractableActor(this);
		
		if (StatueMesh)
		{
			StatueMesh->SetRenderCustomDepth(true); // 외곽선 켜기
		}
		if (InteractionWidgetComponent)
		{
			InteractionWidgetComponent->SetVisibility(true); // 위젯 보이기

		}
	}
}

void AStoneStatue::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(OtherActor))
	{
		// 플레이어에게 상호작용 액션을 해제하라고 알림
		PlayerCharacter->ClearInteractableActor(this);
		// 포커스 종료 (UI 제거 및 외곽선)
		if (StatueMesh)
		{
			StatueMesh->SetRenderCustomDepth(false); // 외곽선 끄기
		}
		if (InteractionWidgetComponent)
		{
			InteractionWidgetComponent->SetVisibility(false); // 위젯 숨기기

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
				// 몽타주 재생
				AnimInstance->Montage_Play(DisableMontage);
			}
		}

		FGameplayEventData Payload;
		Payload.Instigator = InstigatorActor;
		Payload.Target = this;

		// UPROPERTY 변수를 사용하여 이벤트를 전송합니다.
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstigatorActor, InteractionEventTag, Payload);
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionEventTag is not set on %s"), *GetName());
	}

	// 상호작용 후 중복 실행 방지
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (InteractionWidgetComponent) InteractionWidgetComponent->SetVisibility(false);
	if (StatueMesh) StatueMesh->SetRenderCustomDepth(false);
}
	