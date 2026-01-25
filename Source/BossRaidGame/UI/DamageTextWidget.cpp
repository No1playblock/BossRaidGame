// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DamageTextWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h" 

void UDamageTextWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 애니메이션이 있다면 종료 시점 이벤트를 바인딩
    if (FloatAnim)
    {
        // 애니메이션 끝나면 액터 파괴하기 위해 델리게이트 연결
        FWidgetAnimationDynamicEvent AnimFinishedDelegate;
        AnimFinishedDelegate.BindDynamic(this, &UDamageTextWidget::OnAnimFinished);
        BindToAnimationFinished(FloatAnim, AnimFinishedDelegate);
    }
}

void UDamageTextWidget::SetDamageText(float DamageAmount)
{
    if (DamageText)
    {
        // 소수점 버리고 정수로 표시 (원하면 변경 가능)
        DamageText->SetText(FText::AsNumber((int32)DamageAmount));
    }
    if (FloatAnim)
    {
        PlayAnimation(FloatAnim);
    }
}

void UDamageTextWidget::OnAnimFinished()
{
   
    AActor* OwnerActor = GetOwningPlayerPawn(); // 이건 화면 UI일때고, 3D 위젯은 다름

    // 3D 위젯 컴포넌트 방식에서는 사실 Widget 스스로 Actor를 파괴하기 까다로움.
    // 보통은 Actor 쪽에서 LifeSpan을 주거나, 여기서 컴포넌트의 Owner를 찾아 Destroy함
    if (AActor* MyActor = GetTypedOuter<AActor>())
    {
        MyActor->Destroy();
    }
}