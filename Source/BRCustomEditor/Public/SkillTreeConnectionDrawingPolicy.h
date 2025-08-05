#pragma once

#include "CoreMinimal.h"
#include "ConnectionDrawingPolicy.h"

class FSkillTreeConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
    FSkillTreeConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
        : FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements)
    {
        // ���߿� ���ἱ�� �����̳� ����� �ٲٰ� �ʹٸ� �̰����� ����
        // ���� ���, ���� �β��� 5�� ����
        // ConnectionThickness = 5.0f;
    }
};