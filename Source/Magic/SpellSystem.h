// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpellBase.h"
#include "SpellSystem.generated.h"

UCLASS()
class MAGIC_API ASpellSystem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpellSystem(const FObjectInitializer& ObjectInitializer);

	/** The Current Spell ID **/
	UPROPERTY()
	int32 CurrentSpell;

	/** Is Casting? **/
	UPROPERTY()
	bool bCasting;

	/**How long it takes to cast the spell **/
	UPROPERTY()
	float CastTime;

	/** The object reference to the Current Spell **/
	UPROPERTY()
	class ASpellBase* CurrentSpellObject;

	/** Array of all cooldowns in effect **/
	UPROPERTY()
	TArray<float> Cooldowns;

	/** All Spells Available to the owner of the Spell System **/
	UPROPERTY()
	TArray<FSpellStruct> AvailableSpells;

	/** Did Spell Successfully cast? **/
	UPROPERTY()
	bool bSuccess;

	/** Actor Rotation value**/
	UPROPERTY()
	FRotator Rotator;

	/** Person who cast the spell**/
	UPROPERTY()
	class AActor* Caster;

	/** Data of the last spell that was cast**/
	UPROPERTY()
	struct FSpellStruct LastSpell;

	/** Release current spell**/
	UPROPERTY()
	bool bRelease;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	/** Cast the Current Spell in the System **/
	void CastSpell(
		USkeletalMeshComponent* CasterMesh, /** Mesh of actor that cast the spell **/
		FRotator CastRotation,			
		FVector CastPositionOffset,			/** Offset of the Spell from the Owner**/
		class AActor* SpellTarget,				/** Target to adjust aim for, Can be NULL for no target**/
		bool bUseLiveRotation = true		/** Boolean whether to rotate spell with character**/
	);

	/** Add a spell to the spell system **/
	void AddSpell(int32 SpellID);
	/** Remove a spell from the spell system **/
	void RemoveSpell();
	/** Cancel Current spell system cast**/
	void CancelSpellCast();
	/** Select Next Spell in spell system **/
	void SelectNext();
	/** Select Previous Spell in spell system **/
	void SelectPrevious();
	/** Set Release spell of current spell to true Spell in spell system **/
	void ReleaseSpell();
	/** Sets the owner of spell system**/
	void SetOwningPawn(AActor* NewOwner);
protected:
	virtual FVector GetAdjustedAim() const;
public:
	/** Get the Current Selected Spell**/
	FORCEINLINE FSpellStruct GetCurrentSpell() const { return AvailableSpells[CurrentSpell]; }
	/** Get the cooldown of Spell at SpellID**/
	FORCEINLINE float GetCooldown(int32 SpellID) const { return Cooldowns[SpellID]; }
	bool IsCurrentSpellOnCooldown() const;
};
