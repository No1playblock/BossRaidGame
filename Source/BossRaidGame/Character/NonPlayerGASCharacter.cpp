// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NonPlayerGASCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attribute/MobCharacterAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/GASCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Tag/BRGameplayTag.h"
#include "Manager/MobSpawnManager.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Animation/CharacterAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"
ANonPlayerGASCharacter::ANonPlayerGASCharacter()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<UMobCharacterAttributeSet>(TEXT("AttributeSet"));
	GetCharacterMovement()->SetWalkableFloorAngle(5.0f);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // AI�� �����ǰų� ���忡 ��ġ�� �� �ڵ����� ������

	GetCharacterMovement()->bOrientRotationToMovement = true; // �̵� �������� ȸ��
	GetCharacterMovement()->bUseRVOAvoidance = false;

	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel3);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//TODO:
	//BPAICOntroller ����
	//GainExpŬ���� ����
}

UAbilitySystemComponent* ANonPlayerGASCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}
void ANonPlayerGASCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AttributeSet)
	{
		AttributeSet->OnOutOfHealth.AddDynamic(this, &ANonPlayerGASCharacter::OnOutOfHealth);
	}
}

void ANonPlayerGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	MyAIController = NewController;

	ASC->InitAbilityActorInfo(this, this);


	for (const auto& StartAbility : StartAbilities)
	{
		FGameplayAbilitySpec StartSpec(StartAbility);
		ASC->GiveAbility(StartSpec);
	}

}

void ANonPlayerGASCharacter::ActivateCharacter()
{
	if (ASC)
	{
		ASC->CancelAbilities();
		ASC->RemoveLooseGameplayTag(BRTAG_CHARACTER_ISDEAD);
	}
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	if (GetMesh())
	{
		GetMesh()->SetComponentTickEnabled(true);
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->Deactivate();
		GetCharacterMovement()->Activate();
		GetCharacterMovement()->SetComponentTickEnabled(true);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	if (MyAIController)
	{
		MyAIController->Possess(this);
	}
	else
	{
		// ��Ʈ�ѷ��� ���� �������� ��쿡�� ���� ����
		SpawnDefaultController();
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (BehaviorTree)
		{
			AIController->SetActorTickEnabled(true);
			AIController->RunBehaviorTree(BehaviorTree);
		}
	}
}
//�װ��� 5�ʵڿ� ����Ǵ� �Լ�
void ANonPlayerGASCharacter::DeactivateCharacter()
{
	AController* CurrentController = GetController();
	if (CurrentController)
	{
		AAIController* AIController = Cast<AAIController>(CurrentController);
		if (AIController && AIController->GetBrainComponent())
		{
			AIController->GetBrainComponent()->StopLogic(TEXT("Pooled"));
		}
		CurrentController->UnPossess();
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (GetMesh())
	{
		GetMesh()->SetComponentTickEnabled(false);
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Stop(0.0f);
		}
	}
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->SetComponentTickEnabled(false);
	}

	if (AttributeSet)
	{

		AttributeSet->Reset();
	}	
}
void ANonPlayerGASCharacter::SetOwningSpawnManager(AMobSpawnManager* InManager)
{
	OwningSpawnManager = InManager;
}
void ANonPlayerGASCharacter::OnOutOfHealth()
{
	Super::OnOutOfHealth();
	if (ASC)
	{
		ASC->CancelAllAbilities();
		ASC->AddLooseGameplayTag(BRTAG_CHARACTER_ISDEAD);
	}


	AGASCharacterPlayer* Player = Cast<AGASCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (!Player)
		return;
	if (Player)
	{
		//Player->GainExperience(AttributeSet->GetExpReward()); // float ExpReward; ���Ͱ� ���� ���� ����ġ
	}

	UAbilitySystemComponent* PlayerASC = Player->GetAbilitySystemComponent();
	if (PlayerASC)
	{
		// Effect Context ����
		FGameplayEffectContextHandle EffectContext = PlayerASC->MakeEffectContext();
		EffectContext.AddSourceObject(this); // ����ġ ������: �� �� ĳ����

		// Effect Spec ����
		FGameplayEffectSpecHandle SpecHandle = PlayerASC->MakeOutgoingSpec(GainExpEffectClass, 1.f, EffectContext);

		if (SpecHandle.IsValid())
		{

			// SetByCaller �� ����
			//const FGameplayTag XPTag = FGameplayTag::RequestGameplayTag(FName("Data.Experience"));
			SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_EXPERIENCE, AttributeSet->GetExpReward());

			//SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_EXPERIENCE, ExpReward);

			// ����
			PlayerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

		}
	}

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			if (OwningSpawnManager.IsValid())
			{
				OwningSpawnManager->ReturnMonsterToPool(this);
			}
			else
			{
				// ���� �Ŵ����� ���� ��쿡�� �ı� (������ġ)
				Destroy();
			}
		}
	), 5.0f, false);
}

float ANonPlayerGASCharacter::GetDamageByAttackTag(const FGameplayTag& AttackTag) const
{
	if (AttributeSet)
	{
		return AttributeSet->GetAttackPower();
	}
	return 0.0f;
}
