// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/GhostTrailActor.h"
#include "Components/PoseableMeshComponent.h"

AGhostTrailActor::AGhostTrailActor()
{
	PrimaryActorTick.bCanEverTick = false; // 움직일 필요 없으니 틱 끔

	PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMesh"));
	RootComponent = PoseableMesh;
}

void AGhostTrailActor::BeginPlay()
{
	Super::BeginPlay();

	// 0.5초 뒤에 자동 삭제
	SetLifeSpan(0.5f);
}

void AGhostTrailActor::InitGhost(USkeletalMeshComponent* PawnMesh)
{
	if (!PawnMesh || !PoseableMesh) return;

	// 1. 원본 캐릭터의 메쉬와 스킨 정보를 가져옴
	PoseableMesh->SetSkinnedAssetAndUpdate(PawnMesh->GetSkinnedAsset());

	// 2. 현재 프레임의 뼈대 위치(포즈)를 그대로 복사함 (핵심 기능)
	PoseableMesh->CopyPoseFromSkeletalComponent(PawnMesh);

	// 3. 유령 재질 적용 (모든 슬롯에 덮어씌움)
	if (GhostMaterial)
	{
		int32 NumMaterials = PoseableMesh->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials; ++i)
		{
			PoseableMesh->SetMaterial(i, GhostMaterial);
		}
	}
}

