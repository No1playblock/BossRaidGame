// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NonPlayerGASCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attribute/MobCharacterAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/GASCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GA/GA_GainExperience.h"
#include "Tag/BRGameplayTag.h"
ANonPlayerGASCharacter::ANonPlayerGASCharacter()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<UMobCharacterAttributeSet>(TEXT("AttributeSet"));
	
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
}

void ANonPlayerGASCharacter::OnOutOfHealth()
{
	Super::OnOutOfHealth();

	AGASCharacterPlayer* Player = Cast<AGASCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Player)
	{
		//Player->GainExperience(AttributeSet->GetExpReward()); // float ExpReward; ���Ͱ� ���� ���� ����ġ
	}

	UAbilitySystemComponent* PlayerASC = Player->GetAbilitySystemComponent();
        if (PlayerASC)
        {
			UE_LOG(LogTemp, Warning, TEXT("PlayerASC"));
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

				UE_LOG(LogTemp, Warning, TEXT("GiveEXP"));
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
