// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpellBase.h"
#include "SpellDatabaseSingleton.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class USpellDatabaseSingleton : public UObject
{
public:

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell Database")
	TArray<struct FSpellStruct> Spells;

	/** Pointer reference to database itself**/
	UPROPERTY()
		TSubclassOf<class USpellDatabaseSingleton> BP_SpellDatabase;

	USpellDatabaseSingleton(const FObjectInitializer& FObjectInitializer);
};
