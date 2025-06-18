// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraphNode.h"

FText USkillTreeEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    // SkillData에 이름이 있으면 그 이름을, 없으면 기본 제목을 반환
    return SkillData.SkillName.IsEmpty() ? FText::FromString(TEXT("New Skill")) : SkillData.SkillName;
}

void USkillTreeEdGraphNode::CreateInputPin()
{
    // "입력(In)" 핀을 하나 생성합니다. 핀의 타입은 "Skill"로 지정합니다.
    CreatePin(EGPD_Input, FName("Skill"), FName(), FName("In"));
}

void USkillTreeEdGraphNode::CreateOutputPin()
{
    // "출력(Out)" 핀을 하나 생성합니다.
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