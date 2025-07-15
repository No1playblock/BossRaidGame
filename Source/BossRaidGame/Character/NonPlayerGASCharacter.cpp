// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NonPlayerGASCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attribute/MobCharacterAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/GASCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Tag/BRGameplayTag.h"
ANonPlayerGASCharacter::ANonPlayerGASCharacter()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<UMobCharacterAttributeSet>(TEXT("AttributeSet"));
	GetCharacterMovement()->SetWalkableFloorAngle(5.0f);
	//GetCharacterMovement()->bUseRVOAvoidance = true;		정확성이 조금 낮음
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
	AttributeSet->SetExpReward(ExpReward);

	for (const auto& StartAbility : StartAbilities)
	{
		FGameplayAbilitySpec StartSpec(StartAbility);
		ASC->GiveAbility(StartSpec);
	}
}

void ANonPlayerGASCharacter::OnOutOfHealth()
{
	Super::OnOutOfHealth();

	AGASCharacterPlayer* Player = Cast<AGASCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

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
