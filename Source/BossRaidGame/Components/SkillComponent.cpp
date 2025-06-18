
#include "Components/SkillComponent.h"
#include "GameData/SkillTreeData.h" // FSkillTreeDataRow
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Attribute/PlayerCharacterAttributeSet.h" // SkillPoint ��Ʈ����Ʈ

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ������ ���̺� ��� ���� (�����Ϳ��� ���� �����ϵ���)
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
		SkillTreeDataTable = SkillTreeDataTablePath.LoadSynchronous();	//LoadSynchronous �� ���� ����
	}
}

void USkillComponent::UpgradeSkill(const FName& TargetSkillID)
{
	if (!OwnerASC || !SkillTreeDataTable) return;

	// 1. ���׷��̵��Ϸ��� ��ų�� �����͸� ������ ���̺��� ã���ϴ�.
	const FSkillTreeDataRow* TargetSkillData = SkillTreeDataTable->FindRow<FSkillTreeDataRow>(TargetSkillID, TEXT("UpgradeSkill"));	//FindRow �� ���� ����
	if (!TargetSkillData) return;

	// 2. ��ų ����Ʈ�� ������� Ȯ���մϴ�.
	const float CurrentSkillPoints = OwnerASC->GetNumericAttribute(UPlayerCharacterAttributeSet::GetSkillPointAttribute());
	if (CurrentSkillPoints < TargetSkillData->SkillPointCost)
	{
		// ��ų ����Ʈ ����!
		return;
	}

	// 3. ���� ��ų ���ǵ��� ��� �����ߴ��� Ȯ���մϴ�. (���߿� ����)

	// 4. ��ų ����Ʈ�� �Ҹ��մϴ�. (��븸ŭ ���� GameplayEffect�� ����)
	// ...

	FGameplayTag SkillSlotTag = TargetSkillData->SkillSlotTag;

// GetActivatableAbilities()�� �Ķ���� ���� ȣ���Ͽ� ���� �迭�� const ������ ����ϴ�.
const TArray<FGameplayAbilitySpec>& SpecsToFind = OwnerASC->GetActivatableAbilities();

// �迭�� ��ȸ�ϸ� ���� Spec�� Ȯ���մϴ�.
for (const FGameplayAbilitySpec& Spec : SpecsToFind)
{
	// Spec�� �����Ͱ� �ƴϹǷ� -> ��� . �� ����մϴ�.
	if (Spec.Ability && Spec.Ability->AbilityTags.HasTag(SkillSlotTag))
	{
		// ClearAbility�� �ڵ��� �����Ƿ� Spec.Handle�� ����մϴ�.
		OwnerASC->ClearAbility(Spec.Handle);
		break; // �ش� ������ �����Ƽ�� �ϳ��� �ִٰ� �����ϰ� ������ ���������ϴ�.
	}
}
// ...

	// 6. ���ο� �����Ƽ�� �ο��մϴ�.
	FGameplayAbilitySpec NewAbilitySpec(TargetSkillData->GrantedAbility);
	OwnerASC->GiveAbility(NewAbilitySpec);
}