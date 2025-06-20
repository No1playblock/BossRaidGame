// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraphNode.h"

FText USkillTreeEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    // SkillData에 이름이 있으면 그 이름을, 없으면 기본 제목을 반환
    return SkillData.SkillName.IsEmpty() ? FText::FromString(TEXT("New Skill")) : SkillData.SkillName;
}

void USkillTreeEdGraphNode::AllocateDefaultPins()
{
	UE_LOG(LogTemp, Warning, TEXT("Allocate"));

	// 이 노드의 입력 핀과 출력 핀을 생성합니다.
	FEdGraphPinType PinType;
	PinType.PinCategory = FName(TEXT("Skill")); // 우리 핀의 카테고리는 "Skill" 입니다.

	// 2. 생성한 핀 타입을 사용하여 핀을 생성합니다.
	//    노드의 선행 조건이 있을 수 있으므로 입력 핀을 만듭니다.
	CreatePin(EGPD_Input, PinType, FName("In"));

	//    노드가 다음 스킬로 이어질 수 있으므로 출력 핀을 만듭니다.
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