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

	// ü�� ����
	// AttributeSet�� �θ��� UCharacterAttributeSet�� Health, MaxHealth�� �ִٰ� ����
	ASC->SetNumericAttributeBase(AttributeSet->GetMaxHealthAttribute(), MobData->Health);
	ASC->SetNumericAttributeBase(AttributeSet->GetHealthAttribute(), MobData->Health);

	// ����ġ ���� ����
	ASC->SetNumericAttributeBase(AttributeSet->GetExpRewardAttribute(), MobData->Experience);
	ASC->SetNumericAttributeBase(AttributeSet->GetMoveSpeedAttribute(), MobData->WalkSpeed);
	ASC->SetNumericAttributeBase(AttributeSet->GetAttackPowerAttribute(), MobData->AttackPower);
	// �̵� �ӵ� ����
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
		// SetByCaller�� �ٴ� �ӵ� ���� �����ϴ� ���� ����
		SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_SPEED, RunSpeed);

		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}
void AMobGASCharacter::SetWalkingState()
{
	if (!ASC) return;

	// "State.Running" �±׸� �ο��ϴ� ��� Ȱ��ȭ�� ����Ʈ�� ã�Ƽ� ����
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(BRTAG_CHARACTER_ISCHASING);
	ASC->RemoveActiveEffectsWithGrantedTags(TagContainer);
}