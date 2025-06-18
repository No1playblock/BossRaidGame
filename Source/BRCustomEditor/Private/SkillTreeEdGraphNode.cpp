// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraphNode.h"

FText USkillTreeEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    // SkillData�� �̸��� ������ �� �̸���, ������ �⺻ ������ ��ȯ
    return SkillData.SkillName.IsEmpty() ? FText::FromString(TEXT("New Skill")) : SkillData.SkillName;
}

void USkillTreeEdGraphNode::CreateInputPin()
{
    // "�Է�(In)" ���� �ϳ� �����մϴ�. ���� Ÿ���� "Skill"�� �����մϴ�.
    CreatePin(EGPD_Input, FName("Skill"), FName(), FName("In"));
}

void USkillTreeEdGraphNode::CreateOutputPin()
{
    // "���(Out)" ���� �ϳ� �����մϴ�.
    CreatePin(EGPD_Output, FName("Skill"), FName(), FName("Out"));
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