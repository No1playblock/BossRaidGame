// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraphNode.h"

FText USkillTreeEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    // SkillData에 이름이 있으면 그 이름을, 없으면 기본 제목 반환
    return SkillData.SkillName.IsEmpty() ? FText::FromString(TEXT("New Skill")) : SkillData.SkillName;
}

void USkillTreeEdGraphNode::AllocateDefaultPins()
{
	UE_LOG(LogTemp, Warning, TEXT("Allocate"));

	// 이 노드의 입력 핀과 출력 핀을 생성
	FEdGraphPinType PinType;
	PinType.PinCategory = FName(TEXT("Skill")); // 카테고리는 "Skill" 

	//생성한 핀 타입을 사용하여 핀을 생성
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

		// 그래프 UI에 변경 알림
		if (UEdGraph* OwnerGraph = GetGraph())
		{
			OwnerGraph->NotifyGraphChanged();
		}
	}
}