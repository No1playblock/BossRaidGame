// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SkillTreeComponent.h"
#include "GameData/SkillTreeData.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PlayerCharacterAttributeSet.h" // AttributeSet 헤더 포함
#include "Character/GASCharacterPlayer.h"
USkillTreeComponent::USkillTreeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkillTreeComponent::BeginPlay()
{
	Super::BeginPlay();

	// 컴포넌트의 소유자(플레이어 캐릭터)로부터 ASC와 AttributeSet을 찾아 캐싱합니다.
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
}

bool USkillTreeComponent::TryAcquireSkill(FName SkillID)
{
	// 1. 필요한 컴포넌트와 데이터 테이블이 유효한지 확인합니다.
	if (!OwnerASC || !OwnerAttributeSet || !SkillDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("TryAcquireSkill: Component or DataTable is not valid."));
		return false;
	}

	// 2. 데이터 테이블에서 스킬 정보를 찾아옵니다.
	const FSkillTreeDataRow* SkillData = GetSkillData(SkillID);
	if (!SkillData)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryAcquireSkill: SkillID [%s] not found in DataTable."), *SkillID.ToString());
		return false;
	}

	// 3. 이미 배운 스킬인지 확인합니다.
	if (HasLearnedSkill(SkillID))
	{
		UE_LOG(LogTemp, Log, TEXT("TryAcquireSkill: Skill [%s] is already learned."), *SkillID.ToString());
		return false;
	}

	// 4. AttributeSet에서 현재 스킬 포인트를 가져와 비교합니다.
	float CurrentSkillPoints = OwnerAttributeSet->GetSkillPoint();
	if (CurrentSkillPoints < SkillData->SkillPointCost)
	{
		UE_LOG(LogTemp, Log, TEXT("TryAcquireSkill: Not enough skill points. Have %f, Need %d"), CurrentSkillPoints, SkillData->SkillPointCost);
		
		return false;
	}

	// (선택 사항) 여기에 선행 스킬 습득 여부를 체크하는 로직을 추가할 수 있습니다.

	// 5. 모든 조건을 통과했으므로, 스킬 습득을 처리합니다.
	// AttributeSet의 스킬 포인트를 차감합니다.

	OwnerAttributeSet->SetSkillPoint(CurrentSkillPoints - SkillData->SkillPointCost);

	// 습득한 스킬 목록에 추가합니다.
	AcquiredSkills.Add(SkillID);

	// 스킬에 연결된 게임플레이 어빌리티를 부여합니다.
	if (SkillData->GrantedAbility)
	{
		// 만약 부여할 스킬에 InputID가 지정되어 있다면,
		// 동일한 InputID를 가진 기존 스킬을 찾아 제거합니다.
		if (SkillData->InputID != EAbilityInputID::None)
		{
			// 현재 활성화 가능한 모든 어빌리티의 스펙을 가져옵니다.
			TArray<FGameplayAbilitySpec> ActivatableAbilities = OwnerASC->GetActivatableAbilities();
			TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;

			for (const FGameplayAbilitySpec& Spec : ActivatableAbilities)
			{
				// InputID가 동일하고, 새로 배울 어빌리티와 다른 어빌리티인 경우
				if (Spec.InputID == static_cast<int32>(SkillData->InputID) && Spec.Ability != SkillData->GrantedAbility->GetDefaultObject())
				{
					// 제거할 어빌리티의 핸들을 목록에 추가합니다.
					AbilitiesToRemove.Add(Spec.Handle);
				}
			}

			// 목록에 있는 모든 어빌리티를 제거합니다.
			for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToRemove)
			{
				OwnerASC->ClearAbility(Handle); // 핸들을 사용하여 어빌리티를 영구히 제거
				UE_LOG(LogTemp, Log, TEXT("Delete (InputID: %d)"), static_cast<int32>(SkillData->InputID));
			}
		}
		FGameplayAbilitySpec AbilitySpec(SkillData->GrantedAbility, 1); // 레벨 1로 부여

		// 데이터 테이블의 InputID가 'None'이 아니면 스펙에 추가합니다.
		if (SkillData->InputID != EAbilityInputID::None)
		{
			// Enum 값을 int32로 캐스팅하여 InputID에 할당합니다.
			// 이 코드를 통해 어빌리티와 입력이 연결됩니다.
			AbilitySpec.InputID = static_cast<int32>(SkillData->InputID);
		}

		// 최종적으로 InputID가 포함된 스펙으로 어빌리티를 부여합니다.
		OwnerASC->GiveAbility(AbilitySpec);
	}

	// 6. 델리게이트를 호출하여 스킬 습득 성공을 외부에 알립니다.
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