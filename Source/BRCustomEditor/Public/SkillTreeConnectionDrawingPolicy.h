#pragma once

#include "CoreMinimal.h"
#include "ConnectionDrawingPolicy.h"

class FSkillTreeConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
    // �����ڿ��� �θ� Ŭ������ �����ڸ� ȣ���ϱ⸸ �մϴ�.
    FSkillTreeConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
        : FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements)
    {
        // ���߿� ���ἱ�� �����̳� ����� �ٲٰ� �ʹٸ� �̰����� �����մϴ�.
        // ���� ���, ���� �β��� 5�� ����
        // ConnectionThickness = 5.0f;
    }
};