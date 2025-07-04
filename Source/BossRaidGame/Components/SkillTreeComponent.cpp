// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SkillTreeComponent.h"
#include "GameData/SkillTreeData.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PlayerCharacterAttributeSet.h" // AttributeSet ��� ����
#include "Character/GASCharacterPlayer.h"
USkillTreeComponent::USkillTreeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkillTreeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ������Ʈ�� ������(�÷��̾� ĳ����)�κ��� ASC�� AttributeSet�� ã�� ĳ���մϴ�.
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

	// ��ȿ�� �˻�
	if (!OwnerASC || !OwnerAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillTreeComponent: Failed to get Owner's ASC or AttributeSet!"));
	}
}

bool USkillTreeComponent::TryAcquireSkill(FName SkillID)
{
	// 1. �ʿ��� ������Ʈ�� ������ ���̺��� ��ȿ���� Ȯ���մϴ�.
	if (!OwnerASC || !OwnerAttributeSet || !SkillDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("TryAcquireSkill: Component or DataTable is not valid."));
		return false;
	}

	// 2. ������ ���̺��� ��ų ������ ã�ƿɴϴ�.
	const FSkillTreeDataRow* SkillData = GetSkillData(SkillID);
	if (!SkillData)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryAcquireSkill: SkillID [%s] not found in DataTable."), *SkillID.ToString());
		return false;
	}

	// 3. �̹� ��� ��ų���� Ȯ���մϴ�.
	if (HasLearnedSkill(SkillID))
	{
		UE_LOG(LogTemp, Log, TEXT("TryAcquireSkill: Skill [%s] is already learned."), *SkillID.ToString());
		return false;
	}

	// 4. AttributeSet���� ���� ��ų ����Ʈ�� ������ ���մϴ�.
	float CurrentSkillPoints = OwnerAttributeSet->GetSkillPoint();
	if (CurrentSkillPoints < SkillData->SkillPointCost)
	{
		UE_LOG(LogTemp, Log, TEXT("TryAcquireSkill: Not enough skill points. Have %f, Need %d"), CurrentSkillPoints, SkillData->SkillPointCost);
		
		return false;
	}

	// (���� ����) ���⿡ ���� ��ų ���� ���θ� üũ�ϴ� ������ �߰��� �� �ֽ��ϴ�.

	// 5. ��� ������ ��������Ƿ�, ��ų ������ ó���մϴ�.
	// AttributeSet�� ��ų ����Ʈ�� �����մϴ�.

	OwnerAttributeSet->SetSkillPoint(CurrentSkillPoints - SkillData->SkillPointCost);

	// ������ ��ų ��Ͽ� �߰��մϴ�.
	AcquiredSkills.Add(SkillID);

	// ��ų�� ����� �����÷��� �����Ƽ�� �ο��մϴ�.
	if (SkillData->GrantedAbility)
	{
		// ���� �ο��� ��ų�� InputID�� �����Ǿ� �ִٸ�,
		// ������ InputID�� ���� ���� ��ų�� ã�� �����մϴ�.
		if (SkillData->InputID != EAbilityInputID::None)
		{
			// ���� Ȱ��ȭ ������ ��� �����Ƽ�� ������ �����ɴϴ�.
			TArray<FGameplayAbilitySpec> ActivatableAbilities = OwnerASC->GetActivatableAbilities();
			TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;

			for (const FGameplayAbilitySpec& Spec : ActivatableAbilities)
			{
				// InputID�� �����ϰ�, ���� ��� �����Ƽ�� �ٸ� �����Ƽ�� ���
				if (Spec.InputID == static_cast<int32>(SkillData->InputID) && Spec.Ability != SkillData->GrantedAbility->GetDefaultObject())
				{
					// ������ �����Ƽ�� �ڵ��� ��Ͽ� �߰��մϴ�.
					AbilitiesToRemove.Add(Spec.Handle);
				}
			}

			// ��Ͽ� �ִ� ��� �����Ƽ�� �����մϴ�.
			for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToRemove)
			{
				OwnerASC->ClearAbility(Handle); // �ڵ��� ����Ͽ� �����Ƽ�� ������ ����
				UE_LOG(LogTemp, Log, TEXT("Delete (InputID: %d)"), static_cast<int32>(SkillData->InputID));
			}
		}
		FGameplayAbilitySpec AbilitySpec(SkillData->GrantedAbility, 1); // ���� 1�� �ο�

		// ������ ���̺��� InputID�� 'None'�� �ƴϸ� ���忡 �߰��մϴ�.
		if (SkillData->InputID != EAbilityInputID::None)
		{
			// Enum ���� int32�� ĳ�����Ͽ� InputID�� �Ҵ��մϴ�.
			// �� �ڵ带 ���� �����Ƽ�� �Է��� ����˴ϴ�.
			AbilitySpec.InputID = static_cast<int32>(SkillData->InputID);
		}

		// ���������� InputID�� ���Ե� �������� �����Ƽ�� �ο��մϴ�.
		OwnerASC->GiveAbility(AbilitySpec);
	}

	// 6. ��������Ʈ�� ȣ���Ͽ� ��ų ���� ������ �ܺο� �˸��ϴ�.
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

//�Ű����� GA�� ���� ���� ��ȯ
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