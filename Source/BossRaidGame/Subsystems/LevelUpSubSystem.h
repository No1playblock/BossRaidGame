#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "GameData/StatChoiceData.h" // FStatChoiceInfo
#include "LevelUpSubsystem.generated.h"

class UDataTable;
class UUserWidget;
class ULevelUpScreenWidget;
class AGASCharacterPlayer;

UCLASS(Config = Game)
class BOSSRAIDGAME_API ULevelUpSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void ShowLevelUpChoices(AGASCharacterPlayer* PlayerCharacter, int32 NewLevel);
	void ApplyStatChoice(const FStatChoiceInfo& ChoiceInfo);

protected:
	UPROPERTY()
	TObjectPtr<UDataTable> LevelStatDataTable;

	UPROPERTY()
	TSubclassOf<UUserWidget> StatChoiceWidgetClass;

	UPROPERTY(Config)
	TSoftObjectPtr<UDataTable> LevelStatDataTablePath;

	UPROPERTY(Config)
	TSoftClassPtr<UUserWidget> StatChoiceWidgetClassPath;

private:
	void OnDataTableLoaded();

	void OnWidgetClassLoaded();

	UFUNCTION()
	void GamePausedSetting(bool bValue);

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentStatChoiceWidget;

	UPROPERTY()
	TWeakObjectPtr<AGASCharacterPlayer> TargetPlayer;
};