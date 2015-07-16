// Fill out your copyright notice in the Description page of Project Settings.

#include "Magic.h"
#include "SpellBase.h"

// Sets default values
ASpellBase::ASpellBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Use a sphere as simple collision
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollisionSphere"));
	CollisionComp->InitSphereRadius(32.f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bTraceComplexOnMove = true;

	//Players Can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	RootComponent = CollisionComp;

	SpellMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("SpellMovement"));
	SpellMovement->UpdatedComponent = CollisionComp;
	SpellMovement->bRotationFollowsVelocity = true;
	SpellMovement->ProjectileGravityScale = 0.0f;

	SpellEffect = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("SpellEffect"));
	SpellEffect->bAutoDestroy = false;
	SpellEffect->AttachParent = RootComponent;

	SpellMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("ProjectileMesh"));
	SpellMesh->AttachParent = CollisionComp;
	
	/** Spell has no collision at beginning so it can cast**/
	SetActorEnableCollision(false);

	bReplicates = true;
	bReplicateMovement = true;
}

void ASpellBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CollisionComp->OnComponentHit.AddDynamic(this, &ASpellBase::OnHit);
	CollisionComp->MoveIgnoreActors.Add(Instigator);
	PlayerController = GetInstigatorController();
}

// Called every frame
void ASpellBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//If Ready To shoot
	if (bReady)
	{
		//If Scaling and It has been alive less than the casttime
		if (bScaleWhileCasting && (Alive < Spell.CastTime))
		{
			//Scale up the actor
			FVector ScaleVector(Alive / Spell.CastTime);
			SetActorRelativeScale3D(ScaleVector);
		}
		// If Release is true and It has been alive longer than the cast time then release the spell
		if (bRelease && (Alive >= Spell.CastTime))
		{
			ReleaseSpell();
		}
		// If Release is true or it has not been alive less than the CastTime
		if (bRelease || (Alive < CastTime))
		{
			//Increment alive by time
			Alive += DeltaTime;
			if (Alive - CastTime >= Spell.LifeSpan)
			{
				SpellEffect->ToggleActive();
				DisableAndDestroy();
			}
		}
	}
}

void ASpellBase::CastSpell(const FSpellStruct& InSpell)
{
	Spell = InSpell;
	SpellMovement->InitialSpeed = ProjectileSpeed;
	bReady = true;
}

void ASpellBase::ReleaseSpell()
{
	//Play Cast Sound of the Spell
	if (CastSound != NULL)
	{
		UGameplayStatics::PlaySoundAttached((USoundBase*)CastSound, GetRootComponent());
	}

	//Detach the spell
	DetachRootComponentFromParent(true);

	//If live rotation is on change the rotation of spell with actor
	if (bUseLiveRotation)
	{
		FRotator temp = GetOwner()->GetActorRotation();
		SetActorRotation(temp);
	}

	//If there is a target then adjust for target
	if (Target)
	{
		FVector Direction = Target->GetActorLocation() - GetActorLocation();
		InitVelocity(Direction);
	}
	else
	{
		//FVector Velocity = GetActorForwardVector() * ProjectileSpeed;
		FVector Direction = GetActorForwardVector();
		InitVelocity(Direction);
	}

	SetActorEnableCollision(true);
}

void ASpellBase::SetTarget(AActor* InTarget)
{
	if (InTarget != NULL)
	{
		Target = InTarget;
	}
}

void ASpellBase::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		UGameplayStatics::ApplyDamage(OtherActor, Spell.Power, PlayerController.Get(), this, NULL);
		DisableAndDestroy();
	}
}

void ASpellBase::DisableAndDestroy()
{
	SpellMovement->StopMovementImmediately();
	SetLifeSpan(2.0f);
}

void ASpellBase::InitVelocity(FVector& ShootDirection)
{
	if (SpellMovement)
	{
		SpellMovement->Velocity = ShootDirection * SpellMovement->InitialSpeed;
	}
}