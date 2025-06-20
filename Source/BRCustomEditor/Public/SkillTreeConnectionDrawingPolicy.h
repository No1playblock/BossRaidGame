#pragma once

#include "CoreMinimal.h"
#include "ConnectionDrawingPolicy.h"

class FSkillTreeConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
    // 생성자에서 부모 클래스의 생성자를 호출하기만 합니다.
    FSkillTreeConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
        : FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements)
    {
        // 나중에 연결선의 색상이나 모양을 바꾸고 싶다면 이곳에서 수정합니다.
        // 예를 들어, 선의 두께를 5로 설정
        // ConnectionThickness = 5.0f;
    }
};