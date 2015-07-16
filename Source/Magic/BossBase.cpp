// Fill out your copyright notice in the Description page of Project Settings.

#include "Magic.h"
#include "BossBase.h"

// Sets default values
ABossBase::ABossBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	Health = 20.f;
}

// Called when the game starts or when spawned
void ABossBase::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ABossBase::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);
}

float ABossBase::TakeDamage(
	float Damage,
	const FDamageEvent& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0)
		{
			Destroy();
		}
	}

	return ActualDamage;
}
