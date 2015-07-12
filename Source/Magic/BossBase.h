// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "BossBase.generated.h"

USTRUCT()
struct FBossData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Stats)
		float health;
};

UCLASS(abstract)
class MAGIC_API ABossBase : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY()
	struct FBossData BossData;

	
public:
	// Sets default values for this pawn's properties
	ABossBase(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
public:
};
