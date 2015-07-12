// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpellDatabaseSingleton.h"
#include "SingletonFunctionLib.generated.h"

/**
 * 
 */
UCLASS()
class MAGIC_API USingletonFunctionLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Spell Database")
	static USpellDatabaseSingleton* GetDatabase(bool& IsValid);	
};
