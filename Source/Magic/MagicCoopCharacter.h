// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "MagicCoopCharacter.generated.h"

class UInputComponent;

UCLASS(config = Game)
class MAGIC_API AMagicCoopCharacter : public ACharacter
{
	GENERATED_BODY()

	virtual void PostInitializeComponents() override;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
	
	/** Child Actor Component for the Spell System**/
	UPROPERTY()
	class UChildActorComponent* SpellSystemChildComponent;
public:
	AMagicCoopCharacter(const FObjectInitializer& ObjectInitializer);

	/**Spell system**/
	UPROPERTY()
	class ASpellSystem* SpellSystem;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;
	/** Gun muzzle's offset from the characters location */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	/** Fires a projectile. */
	void OnFire();
	/*Release a projectile*/
	void OnRelease();
	/** Handles moving forward/backward */
	void MoveForward(float Val);
	/** Handles stafing movement, left and right */
	void MoveRight(float Val);
	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);
	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

private:
	//Gets the Target from a line trace from eye point straight on.
	//AActor* GetTarget();

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	/** Returns Spell System Child Actor**/
	FORCEINLINE class ASpellSystem* GetSpellSystem() const {return SpellSystem;}
};

