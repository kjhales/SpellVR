// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MagicPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MAGIC_API AMagicPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMagicPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	void OnFire();
};
