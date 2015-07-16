// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "BossBase.generated.h"


UCLASS(abstract)
class MAGIC_API ABossBase : public ACharacter
{
	GENERATED_BODY()	

public:
	// Sets default values for this pawn's properties
	ABossBase(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
protected:
	UPROPERTY(EditAnywhere, Replicated, Category=BossStats)
	float Health;

	/** On Take Damage**/
	virtual float TakeDamage(
		float Damage, 
		const FDamageEvent& DamageEvent, 
		class AController* EventInstigator, 
		class AActor* DamageCauser ) override;
};
