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

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // AI가 스폰되거나 월드에 배치될 때 자동으로 소유됨

	GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 회전
	GetCharacterMovement()->bUseRVOAvoidance = false;

	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel3);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//TODO:
	//BPAICOntroller 적용
	//GainExp클래스 적용
}

UAbilitySystemComponent* ANonPlayerGASCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}


void ANonPlayerGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASC->InitAbilityActorInfo(this, this);

	if (AttributeSet)
	{
		// OnOutOfHealth 델리게이트에 이 클래스의 OnOutOfHealth 함수가 바인딩 되어있는지 확인
		if (!AttributeSet->OnOutOfHealth.IsAlreadyBound(this, &ThisClass::OnOutOfHealth))
		{
			// 바인딩 되어있지 않으면 추가함
			AttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
		}
	}

	for (const auto& StartAbility : StartAbilities)
	{
		FGameplayAbilitySpec StartSpec(StartAbility);
		ASC->GiveAbility(StartSpec);
	}
	CheckNavMeshAndStartAI();

}

void ANonPlayerGASCharacter::ActivateCharacter()
{

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
	SpawnDefaultController();

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (BehaviorTree)
		{
			AIController->RunBehaviorTree(BehaviorTree);
		}
	}
}
void ANonPlayerGASCharacter::CheckNavMeshAndStartAI()
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	AAIController* AIController = Cast<AAIController>(GetController());

	if (!AIController || !BehaviorTree || !NavSys)
	{
		return;
	}

	FNavLocation RandomLocation;
	const bool bIsNavMeshReady = NavSys->GetRandomPointInNavigableRadius(GetActorLocation(), 1.0f, RandomLocation);

	if (bIsNavMeshReady)
	{
		GetWorld()->GetTimerManager().ClearTimer(AIStartTimerHandle);
		AIController->SetActorTickEnabled(true);
		AIController->RunBehaviorTree(BehaviorTree);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(AIStartTimerHandle, this, &ANonPlayerGASCharacter::CheckNavMeshAndStartAI, 0.2f, false);
	}
}
//죽고나서 5초뒤에 실행되는 함수
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
		CurrentController->Destroy();
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
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(BRTAG_CHARACTER_ISDEAD);
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
