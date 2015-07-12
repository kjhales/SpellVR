// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Magic.h"
#include "MagicGameMode.h"
#include "MagicHUD.h"
#include "MagicCoopCharacter.h"

AMagicGameMode::AMagicGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("Pawn '/Game/Magic/Blueprints/BP_MagicCoopCharacter.BP_MagicCoopCharacter_C'"));
	if (PlayerPawnClassFinder.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnClassFinder.Class;
	}
}