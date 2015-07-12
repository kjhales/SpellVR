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
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionComp->InitSphereRadius(32.f);

	//Players Can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	OffSet = FVector(80.0f, 0.0f, -30.0f);

	//Set the colliison as the root component
	RootComponent = CollisionComp;

	SpellMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("SpellMovement"));
	SpellMovement->Velocity = FVector(1.f, 0.f, 0.f);

	SpellEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SpellEffect"));
	SpellEffect->AttachParent = CollisionComp;

	SpellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	SpellMesh->AttachParent = CollisionComp;
	
	SetActorEnableCollision(false);
}

// Called every frame
void ASpellBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//If Ready To shoot
	if (bReady)
	{
		//If Scaling and It has been alive less than the casttime
		if (bScaleWhileCasting && (Alive < CastTime))
		{
			//Scale up the actor
			FVector ScaleVector(Alive / CastTime);
			SetActorRelativeScale3D(ScaleVector);
		}
		// If Release is true and It has been alive longer than the cast time then release the spell
		if (bRelease && (Alive >= CastTime))
		{
			ReleaseSpell();
		}
		// If Release is true or it has not been alive less than the CastTime
		if (bRelease || (Alive < CastTime))
		{
			//Increment alive by time
			Alive += DeltaTime;
			if (Alive - CastTime >= GetLifeSpan())
			{
				SpellEffect->ToggleActive();
				Destroy();
			}
		}
	}
}

void ASpellBase::Cast(struct FSpellStruct InSpell)
{
	Spell = InSpell;
	CastTime = InSpell.CastTime;
	SetLifeSpan(InSpell.LifeSpan);
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
	DetachRootComponentFromParent();

	//If live rotation is on change the rotation of spell with actor
	if (bUseLiveRotation)
	{
		FRotator temp = GetOwner()->GetActorRotation();
		SetActorRotation(temp);
	}

	//If there is a target then adjust for target
	if (Target != NULL)
	{
		FVector Direction = Target->GetActorLocation() - GetActorLocation();
		//SpellMovement->SetVelocityInLocalSpace(Direction * ProjectileSpeed);
		SpellMovement->Velocity = Direction * ProjectileSpeed;
	}
	else
	{
		//SpellMovement->SetVelocityInLocalSpace(GetActorForwardVector() * ProjectileSpeed);
		SpellMovement->Velocity = GetActorForwardVector() * ProjectileSpeed;
	}

	SetActorEnableCollision(true);
}

void ASpellBase::SetTarget(AActor* const InTarget)
{
	if (InTarget != NULL)
	{
		Target = InTarget;
	}
}

void ASpellBase::OnHit_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	UE_LOG(LogTemp, Log, TEXT("Spell Base Hit!"));
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}