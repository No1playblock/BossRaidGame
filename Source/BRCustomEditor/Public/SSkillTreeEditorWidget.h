// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class SGraphEditor; // ���� ����
class USkillTreeEdGraph; // UEdGraph ��� �츮�� ���� Ŭ������ ����
class USkillTreeEdGraphNode; // ��� Ŭ���� ���� ����

class BRCUSTOMEDITOR_API SSkillTreeEditorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillTreeEditorWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
    // ������ ���̺�κ��� ��带 �����ϴ� �Լ�
    void CreateNodesFromDataTable();

    void ConnectNodes();

private:
    // �׷����� ǥ���� ����
    TSharedPtr<SGraphEditor> GraphEditorWidget;

    // ���� �����Ͱ� �����ϰ� �ִ� �׷��� ��ü
    UPROPERTY() // UObject�̹Ƿ� UPROPERTY�� GC ����
        TObjectPtr<USkillTreeEdGraph> SkillGraph;

    // ��ų ID�� ��带 ���� ã�� ���� ��
    TMap<FName, USkillTreeEdGraphNode*> NodeMap;
};
