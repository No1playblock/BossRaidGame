// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NonPlayerGASCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attribute/MobCharacterAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/GASCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Tag/BRGameplayTag.h"
#include "GameData/MobSpawnInfo.h"
ANonPlayerGASCharacter::ANonPlayerGASCharacter()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<UMobCharacterAttributeSet>(TEXT("AttributeSet"));
	GetCharacterMovement()->SetWalkableFloorAngle(5.0f);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // AI�� �����ǰų� ���忡 ��ġ�� �� �ڵ����� ������

	static ConstructorHelpers::FClassFinder<UGameplayEffect> MovementEffectFinder(TEXT("/Game/Blueprints/GE/BPGE_SetStateMovement.BPGE_SetStateMovement_C"));
	if (MovementEffectFinder.Succeeded())
	{
		MovementStateEffectClass = MovementEffectFinder.Class;
	}
}

UAbilitySystemComponent* ANonPlayerGASCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}
void ANonPlayerGASCharacter::InitializeFromData(const FMobSpawnInfo* MobData)
{
	if (!MobData || !AttributeSet || !GetCharacterMovement()) return;

	// ü�� ����
	// AttributeSet�� �θ��� UCharacterAttributeSet�� Health, MaxHealth�� �ִٰ� ����
	ASC->SetNumericAttributeBase(AttributeSet->GetMaxHealthAttribute(), MobData->Health);
	ASC->SetNumericAttributeBase(AttributeSet->GetHealthAttribute(), MobData->Health);

	// ����ġ ���� ����
	ASC->SetNumericAttributeBase(AttributeSet->GetExpRewardAttribute(), MobData->Experience);

	ASC->SetNumericAttributeBase(AttributeSet->GetMoveSpeedAttribute(), MobData->WalkSpeed);

	// �̵� �ӵ� ����
	WalkSpeed = MobData->WalkSpeed;
	RunSpeed = MobData->RunSpeed;
	SetWalkingState();

}

void ANonPlayerGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASC->InitAbilityActorInfo(this, this);

	AttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
	//AttributeSet->SetExpReward(ExpReward);

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMoveSpeedAttribute()).AddUObject(this, &ANonPlayerGASCharacter::OnMovementSpeedChanged);
	}
	for (const auto& StartAbility : StartAbilities)
	{
		FGameplayAbilitySpec StartSpec(StartAbility);
		ASC->GiveAbility(StartSpec);
	}
}
void ANonPlayerGASCharacter::OnMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}
void ANonPlayerGASCharacter::SetChasingState()
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
void ANonPlayerGASCharacter::SetWalkingState()
{
	if (!ASC) return;

	// "State.Running" �±׸� �ο��ϴ� ��� Ȱ��ȭ�� ����Ʈ�� ã�Ƽ� ����
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(BRTAG_CHARACTER_ISCHASING);
	ASC->RemoveActiveEffectsWithGrantedTags(TagContainer);
}
void ANonPlayerGASCharacter::OnOutOfHealth()
{
	Super::OnOutOfHealth();

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
			SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_EXPERIENCE, ExpReward);

			// ����
			PlayerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

		}
	}

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), 5.0f, false);
}
