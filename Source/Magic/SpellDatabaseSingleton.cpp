// Fill out your copyright notice in the Description page of Project Settings.

#include "Magic.h"
#include "SpellDatabaseSingleton.h"

USpellDatabaseSingleton::USpellDatabaseSingleton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> BP_Temp(TEXT("Blueprint'/Game/Magic/Blueprints/BP_SpellDatabaseSingleton.BP_SpellDatabaseSingleton'"));
	if (BP_Temp.Object) {
		BP_SpellDatabase = (UClass*)BP_Temp.Object->GeneratedClass;
	}
}