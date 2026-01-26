// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MobGASCharacter.h"
#include "Tag/BRGameplayTag.h"
#include "Attribute/MobCharacterAttributeSet.h"
#include "GameData/MobSpawnInfo.h"
#include "GameFramework/CharacterMovementComponent.h"

AMobGASCharacter::AMobGASCharacter()
{
	static ConstructorHelpers::FClassFinder<UGameplayEffect> MovementEffectFinder(TEXT("/Game/Blueprints/GE/BPGE_SetStateMovement.BPGE_SetStateMovement_C"));
	if (MovementEffectFinder.Succeeded())
	{
		MovementStateEffectClass = MovementEffectFinder.Class;
	}
}
void AMobGASCharacter::InitializeFromData(const FMobSpawnInfo* MobData)
{
	if (!MobData || !AttributeSet || !GetCharacterMovement()) return;

	// 체력 설정
	// AttributeSet의 부모인 UCharacterAttributeSet에 Health, MaxHealth가 있다고 가정
	ASC->SetNumericAttributeBase(AttributeSet->GetMaxHealthAttribute(), MobData->Health);
	ASC->SetNumericAttributeBase(AttributeSet->GetHealthAttribute(), MobData->Health);

	// 경험치 보상 설정
	ASC->SetNumericAttributeBase(AttributeSet->GetExpRewardAttribute(), MobData->Experience);
	ASC->SetNumericAttributeBase(AttributeSet->GetMoveSpeedAttribute(), MobData->WalkSpeed);
	ASC->SetNumericAttributeBase(AttributeSet->GetAttackPowerAttribute(), MobData->AttackPower);
	// 이동 속도 설정
	WalkSpeed = MobData->WalkSpeed;
	RunSpeed = MobData->RunSpeed;
	SetWalkingState();

}
void AMobGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMoveSpeedAttribute()).AddUObject(this, &AMobGASCharacter::OnMovementSpeedChanged);
	}
}
void AMobGASCharacter::OnMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}
void AMobGASCharacter::SetChasingState()
{
	if (!ASC || !MovementStateEffectClass) return;

	if (ASC->HasMatchingGameplayTag(BRTAG_CHARACTER_ISCHASING)) return;

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(MovementStateEffectClass, 1.f, EffectContext);
	if (SpecHandle.IsValid())
	{
		// SetByCaller로 뛰는 속도 값을 주입하는 것은 동일
		SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_SPEED, RunSpeed);

		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}
void AMobGASCharacter::SetWalkingState()
{
	if (!ASC) return;

	// "State.IsChasing" 태그를 부여하는 모든 활성화된 이펙트를 찾아서 제거
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(BRTAG_CHARACTER_ISCHASING);
	ASC->RemoveActiveEffectsWithGrantedTags(TagContainer);
}