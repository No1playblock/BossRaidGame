// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraph.h"

#include "SkillTreeEdGraph.h"
#include "SkillTreeEdGraphSchema.h" // 우리 커스텀 스키마 헤더 포함

// [추가] 생성자 구현
USkillTreeEdGraph::USkillTreeEdGraph()
{
	// 이 타입의 그래프가 생성될 때, 스키마는 항상 USkillTreeEdGraphSchema 클래스를 사용하도록 지정합니다.
	Schema = USkillTreeEdGraphSchema::StaticClass();
}