// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostTrailActor.generated.h"

UCLASS()
class BOSSRAIDGAME_API AGhostTrailActor : public AActor
{
	GENERATED_BODY()
	
public:
	AGhostTrailActor();

	// 외부에서 이 함수를 호출해서 유령을 세팅함
	void InitGhost(USkeletalMeshComponent* PawnMesh);

protected:
	virtual void BeginPlay() override;

protected:
	// 포즈를 복사할 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UPoseableMeshComponent> PoseableMesh;

	// 유령 재질 (파란색 프레넬 머티리얼)
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UMaterialInterface> GhostMaterial;

};
