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

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // AI가 스폰되거나 월드에 배치될 때 자동으로 소유됨

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

	// 1. 체력 설정
	// AttributeSet의 부모인 UCharacterAttributeSet에 Health, MaxHealth가 있다고 가정
	ASC->SetNumericAttributeBase(AttributeSet->GetMaxHealthAttribute(), MobData->Health);
	ASC->SetNumericAttributeBase(AttributeSet->GetHealthAttribute(), MobData->Health);

	// 2. 경험치 보상 설정
	ASC->SetNumericAttributeBase(AttributeSet->GetExpRewardAttribute(), MobData->Experience);

	ASC->SetNumericAttributeBase(AttributeSet->GetMoveSpeedAttribute(), MobData->WalkSpeed);

	// 3. 이동 속도 설정
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
	// 변경된 새 값(Data.NewValue)을 캐릭터의 실제 이동 속도(MaxWalkSpeed)에 적용
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}
void ANonPlayerGASCharacter::SetChasingState()
{
	if (!ASC || !MovementStateEffectClass) return;

	// GE 블루프린트가 State.Running 태그를 부여하므로, 해당 태그로 중복 적용을 방지
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
void ANonPlayerGASCharacter::SetWalkingState()
{
	if (!ASC) return;

	// "State.Running" 태그를 부여하는 모든 활성화된 이펙트를 찾아서 제거
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
			SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_EXPERIENCE, ExpReward);

			// 적용
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
