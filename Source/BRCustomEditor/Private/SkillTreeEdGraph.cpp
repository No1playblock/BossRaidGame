// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraph.h"

#include "SkillTreeEdGraph.h"
#include "SkillTreeEdGraphSchema.h" // 우리 커스텀 스키마 헤더 포함

USkillTreeEdGraph::USkillTreeEdGraph()
{
	Schema = USkillTreeEdGraphSchema::StaticClass();
}