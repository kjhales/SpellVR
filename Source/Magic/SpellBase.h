// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpellBase.generated.h"

USTRUCT(BlueprintType)
struct FSpellStruct
{
	GENERATED_USTRUCT_BODY()

	/*Spell Class*/
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	TSubclassOf<class ASpellBase> SpellClass;

	/*Name*/
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	FName Name;

	/*Description about the spell*/
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	FText Description;

	/*Texture Icon of spell*/
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	TAssetPtr<UTexture> Icon;

	/*Life Span*/
	UPROPERTY(EditDefaultsOnly, Category = Spell)
	float LifeSpan;

	/*Damage of spell*/
	UPROPERTY(EditDefaultsOnly, Category=SpellStat)
	int32 Power;
	
	/*Amount of mana to cast the spell*/
	UPROPERTY(EditDefaultsOnly, Category=SpellStat)
	int32 Mana;

	/*Cooldown time before recasting spell*/
	UPROPERTY(EditDefaultsOnly, Category=SpellStat)
	float Cooldown;

	/*Time it takes to cast the spell*/
	UPROPERTY(EditDefaultsOnly, Category=SpellStat)
	float CastTime;

	/** Offset of the spell from the caster**/
	UPROPERTY(EditDefaultsOnly)
	FVector CastOffset;

	FSpellStruct()
	{
		SpellClass = NULL;
		Power = 0;
		Mana = 0;
		Cooldown = 0.f;
		CastTime = 0.f;
		LifeSpan = 0.f;
		CastOffset = FVector::ZeroVector;
	}
};

UCLASS(Abstract, Blueprintable)
class MAGIC_API ASpellBase : public AActor
{
	GENERATED_BODY()

	virtual void PostInitializeComponents() override;

	void InitVelocity(FVector& ShootDirection);

private:
	/* Sphere Collision Component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/* Projectile Movement Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* SpellMovement;

	/* Spell Particle System */
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class UParticleSystemComponent* SpellEffect;
	
	/* Spell Mesh */
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class UStaticMeshComponent* SpellMesh;

public:
	// Sets default values for this actor's properties
	ASpellBase(const FObjectInitializer& ObjectInitializer);

	/*How fast the Projectile Travels*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpellStat)
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere, Category = SpellStat)
	float CollisionSize; 

	UPROPERTY()
	float LifeSpan;

	/** How long it takes to cast the spell **/
	float CastTime;

	/** How long has the spell been alive **/
	float Alive;

	/*Sound to play on Cast*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Sound)
	class USoundCue* CastSound;

	/*Struct containing Data of the spell: name, power, manacost, etc*/
	UPROPERTY()
	struct FSpellStruct Spell;

	/*Is the spell ready to release */
	bool bRelease;
	/*Is the spell ready to cast*/
	bool bReady;

	/*Should spell rotate with the player*/
	UPROPERTY(BlueprintReadWrite)
	bool bUseLiveRotation;

	/*Whether the spell should increase in size while Casting*/
	UPROPERTY(BlueprintReadWrite)
	bool bScaleWhileCasting;

	/*Target to hit*/
	UPROPERTY()
	class AActor* Target;

	/** Controller that fired this projectile**/
	TWeakObjectPtr<AController> PlayerController;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	void CastSpell(const FSpellStruct& InSpell);
	void ReleaseSpell();
	void SetTarget(AActor* InTarget);

protected:
	UFUNCTION()
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Shutdown projectile and get ready for destroy**/
	void DisableAndDestroy();
public:
	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return SpellMovement; }
	/** Returns SpellMesh subobject**/
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return SpellMesh;  }
};
