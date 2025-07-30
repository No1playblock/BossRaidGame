// Fill out your copyright notice in the Description page of Project Settings.


#include "BRGameModeBase.h"

ABRGameModeBase::ABRGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> CharacterRef(TEXT("/Game/Blueprints/Character/BP_CharacterPlayer.BP_CharacterPlayer_C"));
	if (CharacterRef.Class)
	{
		DefaultPawnClass = CharacterRef.Class;
	}
}