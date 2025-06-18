// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "SkillTreeEdGraph.generated.h"

/**
 * 
 */
UCLASS()
class BRCUSTOMEDITOR_API USkillTreeEdGraph : public UEdGraph
{
	GENERATED_BODY()
	
public:
	// [추가] 생성자 선언
	USkillTreeEdGraph();
};
