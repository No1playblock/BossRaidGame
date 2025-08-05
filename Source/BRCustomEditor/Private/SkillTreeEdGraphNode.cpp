// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraphNode.h"

FText USkillTreeEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    // SkillData�� �̸��� ������ �� �̸���, ������ �⺻ ���� ��ȯ
    return SkillData.SkillName.IsEmpty() ? FText::FromString(TEXT("New Skill")) : SkillData.SkillName;
}

void USkillTreeEdGraphNode::AllocateDefaultPins()
{
	UE_LOG(LogTemp, Warning, TEXT("Allocate"));

	// �� ����� �Է� �ɰ� ��� ���� ����
	FEdGraphPinType PinType;
	PinType.PinCategory = FName(TEXT("Skill")); // ī�װ��� "Skill" 

	//������ �� Ÿ���� ����Ͽ� ���� ����
	CreatePin(EGPD_Input, PinType, FName("In"));

	CreatePin(EGPD_Output, PinType, FName("Out"));
}


UEdGraphPin* USkillTreeEdGraphNode::GetInputPin() const
{
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->Direction == EGPD_Input)
		{
			return Pin;
		}
	}
	return nullptr;
}

UEdGraphPin* USkillTreeEdGraphNode::GetOutputPin() const
{
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->Direction == EGPD_Output)
		{
			return Pin;
		}
	}
	return nullptr;
}
void USkillTreeEdGraphNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName ChangedPropertyName = PropertyChangedEvent.Property
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(FSkillTreeDataRow, SkillIcon))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillIcon Changed!"));

		// �׷��� UI�� ���� �˸�
		if (UEdGraph* OwnerGraph = GetGraph())
		{
			OwnerGraph->NotifyGraphChanged();
		}
	}
}