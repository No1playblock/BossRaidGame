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

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // AI가 스폰되거나 월드에 배치될 때 자동으로 소유됨

	
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
		//죽으면 OutOfHealth, 5초뒤에 pool로 돌아갈때 태그 때기
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

//죽고나서 5초뒤에 실행되는 함수
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
		// 블렌드 아웃 시간 0.0f로 모든 몽타주를 즉시 정지
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
		//Player->GainExperience(AttributeSet->GetExpReward()); // float ExpReward; 몬스터가 가진 보상 경험치
	}

	UAbilitySystemComponent* PlayerASC = Player->GetAbilitySystemComponent();
	if (PlayerASC)
	{
		// Effect Context 생성
		FGameplayEffectContextHandle EffectContext = PlayerASC->MakeEffectContext();
		EffectContext.AddSourceObject(this); // 경험치 제공자: 이 적 캐릭터

		// Effect Spec 생성
		FGameplayEffectSpecHandle SpecHandle = PlayerASC->MakeOutgoingSpec(GainExpEffectClass, 1.f, EffectContext);

		if (SpecHandle.IsValid())
		{

			// SetByCaller 값 세팅
			//const FGameplayTag XPTag = FGameplayTag::RequestGameplayTag(FName("Data.Experience"));
			SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_EXPERIENCE, AttributeSet->GetExpReward());

			//SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_EXPERIENCE, ExpReward);

			// 적용
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
				// 스폰 매니저가 없는 경우에만 파괴 (안전장치)
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
