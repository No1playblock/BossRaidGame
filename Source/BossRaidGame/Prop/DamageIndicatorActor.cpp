// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/DamageIndicatorActor.h"
#include "Components/WidgetComponent.h" 
#include "UI/DamageTextWidget.h" 
ADamageIndicatorActor::ADamageIndicatorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	DamageWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidgetComp"));
	SetRootComponent(DamageWidgetComp);

	// 항상 카메라를 바라보게 함
	DamageWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	DamageWidgetComp->SetDrawAtDesiredSize(true);
}

void ADamageIndicatorActor::InitializeDamage(float DamageAmount)
{
	if (DamageWidgetComp)
	{
		// 위젯 컴포넌트가 만든 실제 위젯 인스턴스를 가져옴
		if (UDamageTextWidget* WidgetInst = Cast<UDamageTextWidget>(DamageWidgetComp->GetUserWidgetObject()))
		{
			WidgetInst->SetDamageText(DamageAmount);
		}
	}

	// 2초 뒤 강제 파괴
	SetLifeSpan(2.0f);
}

