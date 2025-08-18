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

ANonPlayerGASCharacter::ANonPlayerGASCharacter()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<UMobCharacterAttributeSet>(TEXT("AttributeSet"));
	GetCharacterMovement()->SetWalkableFloorAngle(5.0f);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // AI�� �����ǰų� ���忡 ��ġ�� �� �ڵ����� ������

	
}

UAbilitySystemComponent* ANonPlayerGASCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}


void ANonPlayerGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASC->InitAbilityActorInfo(this, this);

	AttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
	//AttributeSet->SetExpReward(ExpReward);

	
	for (const auto& StartAbility : StartAbilities)
	{
		FGameplayAbilitySpec StartSpec(StartAbility);
		ASC->GiveAbility(StartSpec);
	}
}
void ANonPlayerGASCharacter::ActivateCharacter()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	if (ASC)
	{
		//������ OutOfHealth, 5�ʵڿ� pool�� ���ư��� �±� ����
		UE_LOG(LogTemp, Warning, TEXT("Activating Character: %s"), *GetName());
		ASC->RemoveLooseGameplayTag(BRTAG_CHARACTER_ISDEAD);
	}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	AAIController* AIController = Cast<AAIController>(GetController());
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AIController && BehaviorTree)
	{
		AIController->RunBehaviorTree(BehaviorTree);
	}
}

//�װ��� 5�ʵڿ� ����Ǵ� �Լ�
void ANonPlayerGASCharacter::DeactivateCharacter()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (AttributeSet)
	{
		AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		Cast<UCharacterAnimInstance>(AnimInstance)->SetCharacterDeadState(true);
		// ���� �ƿ� �ð� 0.0f�� ��� ��Ÿ�ָ� ��� ����
		AnimInstance->Montage_Stop(0.0f);

	}
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent());
		if (BehaviorTreeComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("Deactivating Character: %s"), *GetName());
			BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
		}
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
