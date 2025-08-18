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

	// ��ȿ�� �˻�
	if (!OwnerASC || !OwnerAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillTreeComponent: Failed to get Owner's ASC or AttributeSet!"));
	}
	TryAcquireSkill(FName("Player.Skill.AutoRaser.Tier1")); // Q
	TryAcquireSkill(FName("Player.Skill.Grenade.Tier1")); // E
}

bool USkillTreeComponent::TryAcquireSkill(FName SkillID)
{
	// �ʿ��� ������Ʈ�� ������ ���̺��� ��ȿ���� Ȯ��
	if (!OwnerASC || !OwnerAttributeSet || !SkillDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("TryAcquireSkill: Component or DataTable is not valid."));
		return false;
	}

	// ������ ���̺��� ��ų ������ ã�ƿ�.
	const FSkillTreeDataRow* SkillData = GetSkillData(SkillID);
	if (!SkillData)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryAcquireSkill: SkillID [%s] not found in DataTable."), *SkillID.ToString());
		return false;
	}

	// �̹� ��� ��ų���� Ȯ��
	if (HasLearnedSkill(SkillID))
	{
		UE_LOG(LogTemp, Log, TEXT("TryAcquireSkill: Skill [%s] is already learned."), *SkillID.ToString());
		return false;
	}

	// AttributeSet���� ���� ��ų ����Ʈ�� ������ ��
	float CurrentSkillPoints = OwnerAttributeSet->GetSkillPoint();
	if (CurrentSkillPoints < SkillData->SkillPointCost)
	{
		UE_LOG(LogTemp, Log, TEXT("TryAcquireSkill: Not enough skill points. Have %f, Need %d"), CurrentSkillPoints, SkillData->SkillPointCost);
		
		return false;
	}

	// ��ų ������ ó��
	// AttributeSet�� ��ų ����Ʈ�� ����

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
				// InputID�� �����ϰ�, ���� ��� �����Ƽ�� �ٸ� �����Ƽ�� ���
				if (Spec.InputID == static_cast<int32>(SkillData->InputID) && Spec.Ability != SkillData->GrantedAbility->GetDefaultObject())
				{
					// ������ �����Ƽ�� �ڵ��� ��Ͽ� �߰�
					AbilitiesToRemove.Add(Spec.Handle);
				}
			}

			// ��Ͽ� �ִ� ��� �����Ƽ�� ����
			for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToRemove)
			{
				OwnerASC->ClearAbility(Handle); // �ڵ��� ����Ͽ� �����Ƽ�� ������ ����
				UE_LOG(LogTemp, Log, TEXT("Delete (InputID: %d)"), static_cast<int32>(SkillData->InputID));
			}
		}
		FGameplayAbilitySpec AbilitySpec(SkillData->GrantedAbility, 1); // ���� 1�� �ο�

		if (SkillData->InputID != EAbilityInputID::None)
		{

			AbilitySpec.InputID = static_cast<int32>(SkillData->InputID);
		}

		// ���������� InputID�� ���Ե� �������� �����Ƽ�� �ο�
		OwnerASC->GiveAbility(AbilitySpec);
	}

	// ��������Ʈ�� ȣ���Ͽ� ��ų ���� ������ �ܺο� �˸�
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