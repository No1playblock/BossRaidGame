// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "GameplayTagContainer.h" // GameplayTag 사용을 위해 헤더 추가
#include "StoneStatue.generated.h"


class UStaticMeshComponent;
class UUserWidget;
//, public IInteractInterface
UCLASS()
class BOSSRAIDGAME_API AStoneStatue : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStoneStatue();

protected:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void Interact(AActor* InstigatorActor) override;
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> StatueMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USkeletalMeshComponent> StatueAnimalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> InteractionWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UWidgetComponent> InteractionWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	FGameplayTag InteractionEventTag;



private:
	
};
