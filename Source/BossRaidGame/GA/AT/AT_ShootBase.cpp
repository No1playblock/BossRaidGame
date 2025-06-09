// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_ShootBase.h"

void UAT_ShootBase::Activate()
{
	Super::Activate();
	DoShoot();
	EndTask();
}

