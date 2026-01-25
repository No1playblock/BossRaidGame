// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SkillTreeComponent.h"
#include "GameData/SkillTreeData.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PlayerCharacterAttributeSet.h"
#include "Character/GASCharacterPlayer.h"
#include "Player/BRPlayerController.h"
#include "UI/PlayerHUDWidget.h"
USkillTreeComponent::USkillTreeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UDataTable> SkillDataTableRef(TEXT("/Script/Engine.DataTable'/Game/GameData/DT_SkillTree.DT_SkillTree'"));
	if (SkillDataTableRef.Succeeded())
	{
		SkillDataTable = SkillDataTableRef.Object;
		UE_LOG(LogTemp, Log, TEXT("SkillTreeComponent: Loaded Skill DataTable: %s"), *SkillDataTable->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SkillTreeComponent: Failed to load Skill DataTable!"));
	}
}

void USkillTreeComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		OwnerASC = Cast<AGASCharacterPlayer>(Owner)->GetAbilitySystemComponent();
		if (OwnerASC)
		{
			UE_LOG(LogTemp, Log, TEXT("SkillTreeComponent: Found Owner's ASC: %s"), *OwnerASC->GetName());
			OwnerAttributeSet = const_cast<UPlayerCharacterAttributeSet*>(OwnerASC->GetSet<UPlayerCharacterAttributeSet>());
		}
	}

	// 유효성 검사
	if (!OwnerASC || !OwnerAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillTreeComponent: Failed to get Owner's ASC or AttributeSet!"));
	}
	TryAcquireSkill(FName("Player.Skill.AutoRaser.Tier1")); // Q
	TryAcquireSkill(FName("Player.Skill.Grenade.Tier1")); // E
	TryAcquireSkill(FName("Player.Skill.OrbitalStrike.Tier1"));
	TryAcquireSkill(FName("Player.Skill.TacticalDash.Tier1"));

}
const FSkillTreeDataRow* USkillTreeComponent::FindAcquiredSkillByInputID(EAbilityInputID InputID) const
{
	for (const FName& SkillID : AcquiredSkills)
	{
		const FSkillTreeDataRow* SkillData = GetSkillData(SkillID);

		if (SkillData && SkillData->InputID == InputID)
		{
			return SkillData;
		}
	}
	return nullptr;
}
bool USkillTreeComponent::TryAcquireSkill(FName SkillID)
{
	// 필요한 컴포넌트와 데이터 테이블이 유효한지 확인
	if (!OwnerASC || !OwnerAttributeSet || !SkillDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("TryAcquireSkill: Component or DataTable is not valid."));
		return false;
	}

	// 데이터 테이블에서 스킬 정보를 찾아옴.
	const FSkillTreeDataRow* SkillData = GetSkillData(SkillID);
	if (!SkillData)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryAcquireSkill: SkillID [%s] not found in DataTable."), *SkillID.ToString());
		return false;
	}

	// 이미 배운 스킬인지 확인
	if (HasLearnedSkill(SkillID))
	{
		UE_LOG(LogTemp, Log, TEXT("TryAcquireSkill: Skill [%s] is already learned."), *SkillID.ToString());
		return false;
	}

	// AttributeSet에서 현재 스킬 포인트를 가져와 비교
	float CurrentSkillPoints = OwnerAttributeSet->GetSkillPoint();
	if (CurrentSkillPoints < SkillData->SkillPointCost)
	{
		UE_LOG(LogTemp, Log, TEXT("TryAcquireSkill: Not enough skill points. Have %f, Need %d"), CurrentSkillPoints, SkillData->SkillPointCost);
		
		return false;
	}

	// 스킬 습득을 처리
	// AttributeSet의 스킬 포인트를 차감

	OwnerAttributeSet->SetSkillPoint(CurrentSkillPoints - SkillData->SkillPointCost);

	Cast<ABRPlayerController>(Cast<APawn>(GetOwner())->GetController())->GetPlayerHUDWidgetInstance()->SetSkillUI(SkillData->InputID, SkillData);
	AcquiredSkills.Add(SkillID);

	if (SkillData->GrantedAbility)
	{
		if (SkillData->InputID != EAbilityInputID::None)
		{
			TArray<FGameplayAbilitySpec> ActivatableAbilities = OwnerASC->GetActivatableAbilities();
			TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;

			for (const FGameplayAbilitySpec& Spec : ActivatableAbilities)
			{
				// InputID가 동일하고, 새로 배울 어빌리티와 다른 어빌리티인 경우
				if (Spec.InputID == static_cast<int32>(SkillData->InputID) && Spec.Ability != SkillData->GrantedAbility->GetDefaultObject())
				{
					// 제거할 어빌리티의 핸들을 목록에 추가
					AbilitiesToRemove.Add(Spec.Handle);
				}
			}
			// 목록에 있는 모든 어빌리티를 제거
			for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToRemove)
			{
				OwnerASC->ClearAbility(Handle); // 핸들을 사용하여 어빌리티를 영구히 제거
				UE_LOG(LogTemp, Log, TEXT("Delete (InputID: %d)"), static_cast<int32>(SkillData->InputID));
			}
		}
		FGameplayAbilitySpec AbilitySpec(SkillData->GrantedAbility, 1); // 레벨 1로 부여

		if (SkillData->InputID != EAbilityInputID::None)
		{
			AbilitySpec.InputID = static_cast<int32>(SkillData->InputID);
		}

		// 최종적으로 InputID가 포함된 스펙으로 어빌리티를 부여
		OwnerASC->GiveAbility(AbilitySpec);
	}

	// 델리게이트를 호출하여 스킬 습득 성공을 외부에 알림
	OnSkillAcquired.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("TryAcquireSkill: Skill [%s] acquired successfully! Remaining Points: %f"), *SkillID.ToString(), OwnerAttributeSet->GetSkillPoint());
	//UE_LOG(LogTemp, Log, TEXT("Skill [%s] Acquired Successfully! Remaining Points: %f"), *SkillID.ToString(), OwnerAttributeSet->GetSkillPoint());
	return true;
}

bool USkillTreeComponent::HasLearnedSkill(FName SkillID) const
{
	return AcquiredSkills.Contains(SkillID);
}

const FSkillTreeDataRow* USkillTreeComponent::GetSkillData(const FName& SkillID) const
{
	if (!SkillDataTable) return nullptr;
	return SkillDataTable->FindRow<FSkillTreeDataRow>(SkillID, TEXT(""), true);
}

//매개인자 GA랑 같은 열의 반환
const FSkillTreeDataRow* USkillTreeComponent::FindSkillDataByGrantedAbility(TSubclassOf<UGameplayAbility> AbilityClass) const
{
	if (!SkillDataTable || !AbilityClass) return nullptr;

	for (const FName& SkillID : AcquiredSkills)
	{
		const FSkillTreeDataRow* Row = SkillDataTable->FindRow<FSkillTreeDataRow>(SkillID, TEXT(""));
		if (Row && Row->GrantedAbility == AbilityClass)
		{
			return Row;
		}
	}
	return nullptr;
}