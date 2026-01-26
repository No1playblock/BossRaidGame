// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/GhostTrailActor.h"
#include "Components/PoseableMeshComponent.h"

AGhostTrailActor::AGhostTrailActor()
{
	PrimaryActorTick.bCanEverTick = false;

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

	PoseableMesh->SetSkinnedAssetAndUpdate(PawnMesh->GetSkinnedAsset());

	PoseableMesh->CopyPoseFromSkeletalComponent(PawnMesh);

	if (GhostMaterial)
	{
		int32 NumMaterials = PoseableMesh->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials; ++i)
		{
			PoseableMesh->SetMaterial(i, GhostMaterial);
		}
	}
}

