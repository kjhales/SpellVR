// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpellBase.generated.h"

USTRUCT(BlueprintType)
struct FSpellStruct
{
	GENERATED_USTRUCT_BODY()

	/*Spell Class*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ASpellBase> SpellClass;

	/*Name*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name;

	/*Description about the spell*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Description;

	/*Texture Icon of spell*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture* Icon;

	/*Damage of spell*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Power;
	
	/*Amount of mana to cast the spell*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Mana;

	/*Cooldown time before recasting spell*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Cooldown;

	/*Time it takes to cast the spell*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CastTime;

	/*Life Span*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LifeSpan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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

	/* Sphere Collision Component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/* Projectile Movement Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* SpellMovement;

	/* Spell Particle System */
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UParticleSystemComponent* SpellEffect;
	
	/* Spell Mesh */
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class UStaticMeshComponent* SpellMesh;

public:
	// Sets default values for this actor's properties
	ASpellBase(const FObjectInitializer& ObjectInitializer);

	/*How fast the Projectile Travels*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float ProjectileSpeed;

	UPROPERTY(EditAnywhere, Category = Collision)
		float CollisionSize; 

	/*How long it takes to cast the spell*/
	UPROPERTY()
		float CastTime;

	/*How long has the spell been alive*/
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

	/*Offset from the camera location*/
	UPROPERTY()
	FVector OffSet;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	void Cast(struct FSpellStruct InSpell);
	void ReleaseSpell();
	void SetTarget(AActor* const InTarget);

	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay")
		void OnHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
public:
	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return SpellMovement; }
	/** Returns SpellMesh subobject**/
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return SpellMesh;  }
};
