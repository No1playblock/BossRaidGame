
#include "Components/SkillComponent.h"
#include "GameData/SkillTreeData.h" // FSkillTreeDataRow
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Attribute/PlayerCharacterAttributeSet.h" // SkillPoint 어트리뷰트

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 데이터 테이블 경로 설정 (에디터에서 수정 가능하도록)
	static ConstructorHelpers::FObjectFinder<UDataTable> SkillTreeData(TEXT("/Game/Path/To/Your/DT_SkillTree.DT_SkillTree"));
	if (SkillTreeData.Object)
	{
		SkillTreeDataTablePath = SkillTreeData.Object;
	}
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		OwnerASC = OwnerCharacter->FindComponentByClass<UAbilitySystemComponent>();
		if (!OwnerASC)
		{
			if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerCharacter.Get()))
			{
				OwnerASC = ASI->GetAbilitySystemComponent();
			}
		}
	}

	if (SkillTreeDataTablePath.IsValid())
	{
		SkillTreeDataTable = SkillTreeDataTablePath.LoadSynchronous();	//LoadSynchronous 렉 유발 가능
	}
}

void USkillComponent::UpgradeSkill(const FName& TargetSkillID)
{
	if (!OwnerASC || !SkillTreeDataTable) return;

	// 1. 업그레이드하려는 스킬의 데이터를 데이터 테이블에서 찾습니다.
	const FSkillTreeDataRow* TargetSkillData = SkillTreeDataTable->FindRow<FSkillTreeDataRow>(TargetSkillID, TEXT("UpgradeSkill"));	//FindRow 렉 유발 가능
	if (!TargetSkillData) return;

	// 2. 스킬 포인트가 충분한지 확인합니다.
	const float CurrentSkillPoints = OwnerASC->GetNumericAttribute(UPlayerCharacterAttributeSet::GetSkillPointAttribute());
	if (CurrentSkillPoints < TargetSkillData->SkillPointCost)
	{
		// 스킬 포인트 부족!
		return;
	}

	// 3. 선행 스킬 조건들을 모두 만족했는지 확인합니다. (나중에 구현)

	// 4. 스킬 포인트를 소모합니다. (비용만큼 빼는 GameplayEffect를 적용)
	// ...

	FGameplayTag SkillSlotTag = TargetSkillData->SkillSlotTag;

// GetActivatableAbilities()를 파라미터 없이 호출하여 스펙 배열의 const 참조를 얻습니다.
const TArray<FGameplayAbilitySpec>& SpecsToFind = OwnerASC->GetActivatableAbilities();

// 배열을 순회하며 직접 Spec을 확인합니다.
for (const FGameplayAbilitySpec& Spec : SpecsToFind)
{
	// Spec이 포인터가 아니므로 -> 대신 . 를 사용합니다.
	if (Spec.Ability && Spec.Ability->AbilityTags.HasTag(SkillSlotTag))
	{
		// ClearAbility는 핸들을 받으므로 Spec.Handle을 사용합니다.
		OwnerASC->ClearAbility(Spec.Handle);
		break; // 해당 슬롯의 어빌리티는 하나만 있다고 가정하고 루프를 빠져나갑니다.
	}
}
// ...

	// 6. 새로운 어빌리티를 부여합니다.
	FGameplayAbilitySpec NewAbilitySpec(TargetSkillData->GrantedAbility);
	OwnerASC->GiveAbility(NewAbilitySpec);
}