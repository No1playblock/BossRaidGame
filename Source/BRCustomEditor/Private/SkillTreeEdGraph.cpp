// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraph.h"
#include "SkillTreeEdGraphSchema.h"

USkillTreeEdGraph::USkillTreeEdGraph()
{
	Schema = USkillTreeEdGraphSchema::StaticClass();
}