// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraph.h"

#include "SkillTreeEdGraph.h"
#include "SkillTreeEdGraphSchema.h" // �츮 Ŀ���� ��Ű�� ��� ����

// [�߰�] ������ ����
USkillTreeEdGraph::USkillTreeEdGraph()
{
	// �� Ÿ���� �׷����� ������ ��, ��Ű���� �׻� USkillTreeEdGraphSchema Ŭ������ ����ϵ��� �����մϴ�.
	Schema = USkillTreeEdGraphSchema::StaticClass();
}