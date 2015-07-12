// Fill out your copyright notice in the Description page of Project Settings.

#include "Magic.h"
#include "SingletonFunctionLib.h"

USpellDatabaseSingleton* USingletonFunctionLib::GetDatabase(bool& IsValid)
{
	IsValid = false;
	USpellDatabaseSingleton* DatabaseInstance = Cast<USpellDatabaseSingleton>(GEngine->GameSingleton);

	if (!DatabaseInstance) return NULL;
	if (!DatabaseInstance->IsValidLowLevel()) return NULL;

	IsValid = true;
	return DatabaseInstance;
}