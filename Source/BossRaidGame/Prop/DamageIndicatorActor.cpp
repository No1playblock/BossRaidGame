// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/DamageIndicatorActor.h"
#include "Components/WidgetComponent.h" 
#include "UI/DamageTextWidget.h" 
ADamageIndicatorActor::ADamageIndicatorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	DamageWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidgetComp"));
	SetRootComponent(DamageWidgetComp);

	// 중요 설정: 3D 공간에 있지만, 항상 카메라를 바라보게 함 (빌보드)
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

	// 안전장치: 애니메이션 이벤트가 씹힐 경우를 대비해 2초 뒤 강제 파괴
	SetLifeSpan(2.0f);
}

